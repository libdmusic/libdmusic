if($args.Count -ne 2) {
    Write-Error "USAGE: convert_styles.ps1 [Gothic Directory] [REGoth Directory]"
    exit 1
}

if(!(Test-Path $args[0]) -or !(Test-Path $args[1])) {
    Write-Error "Error: directory does not exisits";
    exit 1
}

$MUSICPATH=$args[0] + "/_work/Data/Music"


if(!(Test-Path $MUSICPATH)) {
    Write-Error "Error: invalid directory specified";
    exit 1
}

mkdir -Force ($args[1] + "/soundfonts")

foreach ($worldDirectory in Get-ChildItem -dir -Path $MUSICPATH/*) {
    $worldName = $worldDirectory.name
    Write-Information "Converting \*.sty and \*.sgt files in directory $worldName"
    $stys = (Get-ChildItem $worldDirectory/*.sty | ForEach-Object { "$_" })
    $sgts = (Get-ChildItem $worldDirectory/*.sgt | ForEach-Object { "$_" })
    ./dls2sf -s $stys $sgts -o "$($args[1] + "/soundfonts")" -i $worldDirectory
}