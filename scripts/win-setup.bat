@echo off
pushd %~dp0..\
echo Updating submodules, this may take some time.
call git submodule update --progress
call git submodule init
popd
call python python/setup.py
pause