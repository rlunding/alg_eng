Function Get-DataGenerator
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
        [ValidateRange(0.0, 0.4)]
        [double]$Fluctuation = 0.1,
        [Parameter(Mandatory = $false)]
        [System.Random]$RandomGen = [System.Random]::new(),
        [Parameter(Mandatory = $false)]
        [string]$FormatString = 'START /REALTIME /WAIT datagen {0} data/{1}'
    )
    $From..$To | ForEach-Object {
        If ($_ -eq 29)
        {
            $local:size = 1 - $RandomGen.NextDouble() * $Fluctuation;
        }
        else
        {
            $local:size = ($RandomGen.NextDouble() * 2 - 1) * $Fluctuation + 1;
        }
        $local:size = $local:size * [System.Math]::Pow(2, $_);
        $local:size = [long]$local:size;
        $local:output = [string]::Format($FormatString, $local:size, $_);
        $local:output | Write-Output;
    } | Write-Output;
}
