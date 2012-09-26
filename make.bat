@ECHO OFF

if "%1" == "setup" (
   python setup.py py2exe
   goto end
)

if "%1" == "push" (
   git push origin master
   goto end
)

if "%1" == "pull" (
   git pull origin master
   goto end
)

if "%1" == "build" (
   cd settings_server
   go build settings_server.go
   goto end
)
:end