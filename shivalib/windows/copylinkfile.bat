@echo off

if exist "debug\liblibshiva.a" (
  if not exist "debug\libshiva.a" (
    copy "debug\liblibshiva.a" "debug\libshiva.a"
  ) else (
    xcopy /D /Y "debug\liblibshiva.a" "debug\libshiva.a"
  ) 
)

if exist "release\liblibshiva.a" (
  if not exist "release\libshiva.a" (
    copy "release\liblibshiva.a" "release\libshiva.a"
  ) else (
    xcopy /D /Y "release\liblibshiva.a" "release\libshiva.a"
  ) 
)
