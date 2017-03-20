Function Get-BenchmarkCode
{
    Param
    (
        [Parameter(Mandatory = $false)]
        [ValidateRange(11, 29)]
        [uint32]$From = 11,
        [Parameter(Mandatory = $false)]
        [ValidateRange(11, 29)]
        [uint32]$To = 29,
        [Parameter(Mandatory = $false)]
        [uint32[]]$Seeds = @(24601, 271828, 314159),
        [Parameter(Mandatory = $false)]
        [string]$DataFormat = 'data\{0}',
        [Parameter(Mandatory = $false)]
        [string]$ResultFormat = 'results\{0}\result',
        [Parameter(Mandatory = $false)]
        [ValidateRange(10000, 5000000)]
        [uint32]$QueryCount = 1000000,
        [Parameter(Mandatory = $false)]
        [ValidateRange(10000, 5000000)]
        [uint32]$RefreshCount = 1000000,
        [Parameter(Mandatory = $false)]
        [ValidateRange(10, 600)]
        [uint32]$Timeout = 600
    )
    '@ECHO OFF' | Write-Output;
    'rspcm' | Write-Output;
    'TIMEOUT /T 10' | Write-Output;
    'CLS' | Write-Output;
    $local:dataString = ($From..$To | ForEach-Object { [string]::Format($DataFormat, $_) }) -join ' ';
    $Seeds | ForEach-Object {
        $local:resultString = [string]::Format($ResultFormat, $_);
        $local:resultDirectory = [System.IO.Path]::GetDirectoryName($local:resultString);
        if (-not [string]::IsNullOrWhiteSpace($local:resultDirectory))
        {
            "MD $local:resultDirectory" | Write-Output;
        }
        "START /REALTIME /AFFINITY 0x1 /B /WAIT bmpcm $local:dataString $local:resultString -q:$QueryCount -r:$RefreshCount -s:$_ 2>NUL" | Write-Output;
        "ECHO ERRORLEVEL=%ERRORLEVEL%" | Write-Output;
        "TIMEOUT /T $Timeout" | Write-Output;
    } | Write-Output;
}
