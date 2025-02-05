@echo OFF
mkdir "Resources/Textures/Compiled"
for /r %%f in (Resources\Textures\Sources\*) do (
	"./Tools/texconv.exe" "%%f" -o "Resources/Textures/Compiled" -nologo -y -srgbi -pmalpha -m 4
)