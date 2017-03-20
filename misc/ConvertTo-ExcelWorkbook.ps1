Function ConvertTo-ExcelWorkbook
{
    Param
    (
        [Parameter(Mandatory = $false)]
        [ValidateSet('pcm', 'papi')]
        [string]$ResultExtension = 'pcm',
        [Parameter(Mandatory = $false)]
        [string]$Path = '.'
    )
    <# Parses the result. #>
    Write-Verbose "$([DateTime]::Now.ToString('[yyyy-MM-dd HH:mm:ss]')) Parsing results.";
    $results = @{};
    Get-ChildItem $Path |
    ForEach-Object -Begin { $replacer = [regex]::new('^.*?\.' + $ResultExtension + '\.(.*?)$'); } {
        If ($replacer.IsMatch($_.Name))
        {
            $filename = $replacer.Replace($_.Name, '$1');
            $results[$filename] = Get-Content $_.FullName | ConvertFrom-Csv;
        }
    };
    $minimax = $results.Keys | ForEach-Object { $results[$_] | ForEach-Object { $_.LOG2N } } | Measure-Object -Maximum -Minimum;
    Write-Verbose "$([DateTime]::Now.ToString('[yyyy-MM-dd HH:mm:ss]')) - Finished.";

    $excel = New-Object -ComObject 'Excel.Application';
    $workbook = $excel.Workbooks.Add();
    $excelCalculation = $excel.Calculation;
    $excel.Calculation = -4135; <# suspends automatic calculation #>
    Write-Verbose "$([DateTime]::Now.ToString('[yyyy-MM-dd HH:mm:ss]')) Switched to manual computation.";

    <# Comparison of a measure among algorithms. #>
    $CreateSheetFor = {
        Param
        (
            $Worksheet,
            $SheetTitle,
            $ChartTitle,
            $Property
        )
        Write-Verbose "$([DateTime]::Now.ToString('[yyyy-MM-dd HH:mm:ss]')) Creating comparison of $SheetTitle among algorithms.";
        $Worksheet.Parent.Activate();
        $Worksheet.Activate();
        $Worksheet.Name = $SheetTitle;
        $Worksheet.Shapes.AddChart().Select();

        $local:activeChart = $Worksheet.Application.ActiveChart;
        $activeChart.ChartType = 75; <# scatter lines without markers #>
        $activeChart.Parent.Left = 0;
        $activeChart.Parent.Top = 0;
        $activeChart.Parent.Width = 500;
        $activeChart.Parent.Height = 375;
        $activeChart.Axes(1).MinimumScale = [Math]::Floor($minimax.Minimum);
        $activeChart.Axes(1).MaximumScale = [Math]::Ceiling($minimax.Maximum);
        $activeChart.HasLegend = $false;
        $activeChart.HasTitle = $true;
        $activeChart.ChartTitle.Text = $ChartTitle;

        $local:seriesCollection = $activeChart.SeriesCollection();
        $results.Keys | Sort-Object | ForEach-Object -Begin { $local:row = 1; } {
            $results[$_] | Sort-Object -Property 'LOG2N' | ForEach-Object -Begin { $local:endRow = $row; } {
                $Worksheet.Cells($endRow, 1) = $_.LOG2N;
                $Worksheet.Cells($endRow, 2) = $_.$Property;
                ++$endRow;
            };
            $series = $seriesCollection.NewSeries();
            $series.Name = $_;
            $series.MarkerStyle = 0;
            $series.XValues = $Worksheet.Range("A$($row):A$($endRow - 1)");
            $series.Values = $Worksheet.Range("B$($row):B$($endRow - 1)");
            $local:lastPoint = $series.Points($endRow - $row);
            $lastPoint.HasDataLabel = $true;
            $lastPoint.DataLabel.ShowValue = $false;
            $lastPoint.DataLabel.ShowSeriesName = $true;
            $row = $endRow;
        };
        Write-Verbose "$([DateTime]::Now.ToString('[yyyy-MM-dd HH:mm:ss]')) - Finished.";
    };

    <# Linear regression for CPU cycle consumed by measures. #>
    $CreateSheetFor2 = {
        Param
        (
            $Worksheet,
            $SheetTitle
        )
        Write-Verbose "$([DateTime]::Now.ToString('[yyyy-MM-dd HH:mm:ss]')) Creating linear regression worksheet.";
        $Worksheet.Parent.Activate();
        $Worksheet.Activate();
        $Worksheet.Name = $SheetTitle;

        $local:startRow = 10;
        $local:row = $startRow;
        $Worksheet.Cells($row, 1) = 'Log2(N)';
        $Worksheet.Cells($row, 2) = 'L1 Misses';
        $Worksheet.Cells($row, 3) = 'L2 Misses';
        $Worksheet.Cells($row, 4) = 'L3 Misses';
        $Worksheet.Cells($row, 5) = 'Instructions';
        $Worksheet.Cells($row, 6) = 'Mispredictions';
        $Worksheet.Cells($row, 7) = 'Cycles';
        ++$row;
        $results.Keys | ForEach-Object {
            $results[$_] | ForEach-Object {
                $Worksheet.Cells($row, 11) = $_.LOG2N;
                $Worksheet.Cells($row, 12) = $_.L1_MISS;
                $Worksheet.Cells($row, 13) = $_.L2_MISS;
                $Worksheet.Cells($row, 14) = $_.L3_MISS;
                $Worksheet.Cells($row, 15) = $_.INSTRUCTIONS;
                $Worksheet.Cells($row, 16) = $_.BR_MISPRED;
                $Worksheet.Cells($row, 17) = $_.CYCLES;
                ++$row;
            };
        };
        $Worksheet.Cells(1, 1) = 'Use column for regression?';
        $Worksheet.Range('A2:G2') = '1';
        $Worksheet.Cells(4, 1) = 'Coefficients';
        $Worksheet.Cells(5, 1).Formula = '=$P$5';
        $Worksheet.Cells(5, 2).Formula = '=$O$5';
        $Worksheet.Cells(5, 3).Formula = '=$N$5';
        $Worksheet.Cells(5, 4).Formula = '=$M$5';
        $Worksheet.Cells(5, 5).Formula = '=$L$5';
        $Worksheet.Cells(5, 6).Formula = '=$K$5';
        $Worksheet.Range("A$($startRow + 1):G$($row - 1)").FormulaR1C1 = '=R[0]C[10]*R2C[0]';
        $Worksheet.Range('K5:P5').FormulaArray = "=LINEST(`$G`$$($startRow + 1):`$G`$$($row - 1),`$A`$$($startRow + 1):`$F`$$($row - 1),FALSE,FALSE)";
        $Worksheet.Cells($startRow, 8) = 'Error';
        $Worksheet.Range("H$($startRow + 1):H$($row - 1)").FormulaR1C1 = '=SUMPRODUCT(R[0]C1:R[0]C6,R5C1:R5C6)-R[0]C7';
        $Worksheet.Cells($startRow, 9) = 'Relative Error';
        $Worksheet.Range("I$($startRow + 1):I$($row - 1)").FormulaR1C1 = '=R[0]C8/R[0]C7';
        $Worksheet.Cells(7, 7) = 'Avg';
        $Worksheet.Cells(8, 7) = 'Res';
        $Worksheet.Cells(7, 8).Formula = "=(SUMIF(`$H`$$($startRow + 1):`$H`$$($row - 1),`">0`") - SUMIF(`$H`$$($startRow + 1):`$H`$$($row - 1),`"<0`")) / $($row - $startRow - 1)";
        $Worksheet.Cells(8, 8).Formula = "=SQRT(SUMPRODUCT(`$H`$$($startRow + 1):`$H`$$($row - 1), `$H`$$($startRow + 1):`$H`$$($row - 1)) / $($row - $startRow - 1))";
        $Worksheet.Cells(7, 9).Formula = "=(SUMIF(`$I`$$($startRow + 1):`$I`$$($row - 1),`">0`") - SUMIF(`$I`$$($startRow + 1):`$I`$$($row - 1),`"<0`")) / $($row - $startRow - 1)";
        $Worksheet.Cells(8, 9).Formula = "=SQRT(SUMPRODUCT(`$I`$$($startRow + 1):`$I`$$($row - 1), `$I`$$($startRow + 1):`$I`$$($row - 1)) / $($row - $startRow - 1))";
        Write-Verbose "$([DateTime]::Now.ToString('[yyyy-MM-dd HH:mm:ss]')) - Finished.";
    };

    <# Comparison of contribution of cycles of measures for single algorithms. #>
    $CreateSheetFor3 = {
        Param
        (
            $Worksheet,
            $SheetTitle,
            $ChartTitle,
            $Algorithm
        )
        Write-Verbose "$([DateTime]::Now.ToString('[yyyy-MM-dd HH:mm:ss]')) Creating doughnut chart for $Algorithm.";
        $Worksheet.Parent.Activate();
        $Worksheet.Activate();
        $Worksheet.Name = $SheetTitle;
        $Worksheet.Shapes.AddChart().Select();

        $local:activeChart = $Worksheet.Application.ActiveChart;
        $activeChart.ChartType = -4120; <# doughnut #>
        $activeChart.Parent.Left = 0;
        $activeChart.Parent.Top = 0;
        $activeChart.Parent.Width = 500;
        $activeChart.Parent.Height = 375;
        $activeChart.HasLegend = $true;
        $activeChart.HasTitle = $true;
        $activeChart.ChartTitle.Text = $ChartTitle;

        $Worksheet.Cells(1, 1) = 'Log2(N)';
        $Worksheet.Cells(1, 2) = 'L1 Misses';
        $Worksheet.Cells(1, 3) = 'L2 Misses';
        $Worksheet.Cells(1, 4) = 'L3 Misses';
        $Worksheet.Cells(1, 5) = 'Instructions';
        $Worksheet.Cells(1, 6) = 'Mispredictions';
        $Worksheet.Cells(1, 7) = 'Other';
        $local:row = 2;
        $results[$Algorithm] | ForEach-Object {
            $Worksheet.Cells($row, 11) = $_.LOG2N;
            $Worksheet.Cells($row, 12) = $_.L1_MISS;
            $Worksheet.Cells($row, 13) = $_.L2_MISS;
            $Worksheet.Cells($row, 14) = $_.L3_MISS;
            $Worksheet.Cells($row, 15) = $_.INSTRUCTIONS;
            $Worksheet.Cells($row, 16) = $_.BR_MISPRED;
            $Worksheet.Cells($row, 17) = $_.CYCLES;
            ++$row;
        };
        $Worksheet.Range("A2:F$($row - 1)").FormulaR1C1 = '=R[0]C[10]*Regression!R5C[0]';
        $Worksheet.Range("G2:G$($row - 1)").FormulaR1C1 = '=R[0]C17-SUM(R[0]C1:R[0]C6)';
        $activeChart.SetSourceData($Worksheet.Range("A1:G$($row - 1)"), 1); <# plot by rows. #>
        Write-Verbose "$([DateTime]::Now.ToString('[yyyy-MM-dd HH:mm:ss]')) - Finished.";
    };

    & $CreateSheetFor2 -Worksheet $excel.ActiveSheet -SheetTitle 'Regression';
    $results.Keys | Sort-Object | ForEach-Object {
        & $CreateSheetFor3 -Worksheet $workbook.Sheets.Add() -SheetTitle $_ -ChartTitle "Contribution of Cycles ($_)" -Algorithm $_;
    };
    & $CreateSheetFor -Worksheet $workbook.Sheets.Add() -SheetTitle 'Cycles' -ChartTitle 'CPU Cycles / Query' -Property 'CYCLES';
    & $CreateSheetFor -Worksheet $workbook.Sheets.Add() -SheetTitle 'Mispredictions' -ChartTitle 'Branch Mispredictions / Query' -Property 'BR_MISPRED';
    & $CreateSheetFor -Worksheet $workbook.Sheets.Add() -SheetTitle 'Instructions' -ChartTitle 'Instructions / Query' -Property 'INSTRUCTIONS';
    & $CreateSheetFor -Worksheet $workbook.Sheets.Add() -SheetTitle 'L3 Cache' -ChartTitle 'L3 Cache Misses / Query' -Property 'L3_MISS';
    & $CreateSheetFor -Worksheet $workbook.Sheets.Add() -SheetTitle 'L2 Cache' -ChartTitle 'L2 Cache Misses / Query' -Property 'L2_MISS';
    & $CreateSheetFor -Worksheet $workbook.Sheets.Add() -SheetTitle 'L1 Cache' -ChartTitle 'L1 Cache Misses / Query' -Property 'L1_MISS';

    $excel.Calculation = $excelCalculation; <# reverts to normal calculation #>
    Write-Verbose "$([DateTime]::Now.ToString('[yyyy-MM-dd HH:mm:ss]')) Switched computation mode back.";
    $excel.Visible = $true;
    Write-Verbose "$([DateTime]::Now.ToString('[yyyy-MM-dd HH:mm:ss]')) Showing Excel.";
}
