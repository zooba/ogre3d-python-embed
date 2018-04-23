
$pver = & py -3.6-32 -V
if (-not $?) {
    Write-Host "Please install Python 3.6 32-bit (including the launcher)"
    Write-Host " ** https://www.python.org/ftp/python/3.6.5/python-3.6.5-webinstall.exe"
    Write-Host ""
    Write-Host "After installing, rerun this script."
    exit 1
}

$tmp = mkdir -Force tmp

Write-Host "Getting ogre3d SDK (120MB) - https://www.ogre3d.org/download/sdk"
if (Test-Path "ogre") {
    del -r -fo ogre
}
if (-not (Test-Path "$tmp\ogre.zip")) {
    iwr "https://bintray.com/ogrecave/ogre/download_file?file_path=ogre-sdk-master-vc14-x64.zip" -OutFile "$tmp\ogre.zip"
}
py -3.6-32 -m zipfile -e "$tmp\ogre.zip" ogre

Write-Host "Getting SDL2 dev libraries - https://www.libsdl.org/download-2.0.php"
if (Test-Path "sdl") {
    del -r -fo sdl
}
if (-not (Test-Path "$tmp\sdl2.zip")) {
    iwr "https://www.libsdl.org/release/SDL2-devel-2.0.8-VC.zip" -OutFile "$tmp\sdl2.zip"
}
py -3.6-32 -m zipfile -e "$tmp\sdl2.zip" sdl

Write-Host ""
if (Test-Path "$env:SystemRoot\SysWOW64\d3dx9_43.dll") {
    Write-Host "DirectX redistributable is already installed"
} else {
    Write-Host "Please install the DirectX redistributable"
    Write-Host " ** https://www.microsoft.com/en-us/download/details.aspx?id=35"
}
