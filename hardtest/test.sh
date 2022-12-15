python3 buttonDetector.py

if ["$?" == "1"]
then
    echo Pressed
    python3 ledPressed.py
else
    echo Release
    python3 ledRelease.py
fi
