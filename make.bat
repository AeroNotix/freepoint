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
:end