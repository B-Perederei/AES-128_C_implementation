  param (
    [Parameter(Mandatory=$true)][string]$path,
    [Parameter(Mandatory=$true)][string[]]$subFolder
 )
 if (-not (Test-Path -Path $path -PathType Container))
 {
    throw "Path does not exist"
 }

 foreach ($dir in $subFolder)
 {
    Join-Path $path $dir | Set-Variable -Name "subDir"
    if (-not (Test-Path -Path $subDir -PathType Container))
    {
      New-Item -ItemType "directory" -Path $subDir
    }
 }