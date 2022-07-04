from . import dependencies
from . import utils
from . import tasks
from . import compileResourcePack
import importlib

class CreateDirs(tasks.Task):
    def execute(self):
        utils.createDir(self.project_path + "Dependencies/")
        utils.createDir(self.project_path + "Output/Windows/")

class InstallZlib(tasks.Task):
    def checkForDependencies(self):
        if not utils.exists("C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/Tools/VsDevCmd.bat"):
            raise Exception("C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/Tools/VsDevCmd.bat is needed for build. It is installed alongside visual studio 2022")

    def execute(self):
        dependencies.installDependency("https://github.com/madler/zlib/archive/refs/heads/master.zip", self.project_path + "Dependencies/zlib-master/", "zlib", f"\"\"C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/Tools/VsDevCmd.bat\" && cd {self.project_path}Dependencies/zlib-master/ && cmake -DCMAKE_INSTALL_PREFIX=. -G \"Visual Studio 17 2022\" -A Win32 . && cmake --build . --config Release --target install\"")

class InstallGlfw(tasks.Task):
    def execute(self):
        dependencies.installDependency("https://github.com/glfw/glfw/releases/download/3.3.7/glfw-3.3.7.bin.WIN32.zip", self.project_path + "Dependencies/glfw-3.3.7.bin.WIN32/", "glfw")


class InstallPatch(tasks.Task):
    def execute(self):
        dependencies.installDependency("https://netix.dl.sourceforge.net/project/gnuwin32/patch/2.5.9-7/patch-2.5.9-7-bin.zip", self.project_path + "Dependencies/patch-2.5.9-7-bin/", "patch", create_dir=True)

class GenerateGlad(tasks.Task):
    def checkForDependencies(self):
        glad_spec = importlib.util.find_spec("glad")
        if glad_spec is None:
            raise Exception("Glad needs to be installed as a python package, you can install it via pip")

    def execute(self):
        if not utils.exists(self.project_path + "Dependencies/glad/"):
            utils.system(f"py -m glad --profile compatibility --out-path \"{self.project_path}Dependencies/glad/\" --api gl=4.6 --generator c")
        else:
            print("Glad already generated")


class InstallPlatformFolders(tasks.Task):
    def execute(self):
        dependencies.installDependency("https://github.com/sago007/PlatformFolders/archive/refs/tags/4.2.0.zip", self.project_path + "Dependencies/PlatformFolders-4.2.0/", "platform folders")


class InstallPerlinNoise(tasks.Task):
    def execute(self):
        dependencies.installDependency("https://github.com/Reputeless/PerlinNoise/archive/refs/tags/v3.0.0.zip", self.project_path + "Dependencies/PerlinNoise-3.0.0/", "perlin noise")


class CompileResourcePack(tasks.Task):
    def execute(self):
        compileResourcePack.compileResourcePack(self.project_path + "Resources", self.project_path + "Build/Resources/")


class Build(tasks.Task):
    def checkForDependencies(self):
        if not utils.exists("C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/Tools/VsDevCmd.bat"):
            raise Exception("C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/Tools/VsDevCmd.bat is needed for build. It is installed alongside visual studio 2022")

    def execute(self):
        utils.system(f"\"\"C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/Tools/VsDevCmd.bat\" && cd {self.project_path}Build/ && cmake -DCMAKE_BUILD_TYPE=Release -G \"CodeBlocks - NMake Makefiles\" .. && cmake --build .\"")

class UnpackClient(tasks.Task):
    def checkForDependencies(self):
        self.requireCommand("xcodebuild")

    def execute(self):
        utils.remove(self.project_path + "Output/MacOS/Terralistic.app/")
        utils.system(f"xcodebuild -exportArchive -quiet -archivePath {self.project_path}Terralistic.xcarchive -exportPath {self.project_path}Terralistic.app/ -exportOptionsPlist {self.project_path}Terralistic.xcodeproj/exportOptions.plist")
        utils.copy(f"{self.project_path}Terralistic.app/Terralistic.app/", f"{self.project_path}Output/MacOS/Terralistic.app/")
        utils.remove(self.project_path + "Terralistic.xcarchive/")
        utils.remove(self.project_path + "Terralistic.app/")


class UnpackServer(tasks.Task):
    def checkForDependencies(self):
        self.requireCommand("xcodebuild")

    def execute(self):
        utils.remove(self.project_path + "Output/MacOS/Terralistic-server.app/")
        utils.system(f"xcodebuild -exportArchive -quiet -archivePath {self.project_path}Terralistic-server.xcarchive -exportPath {self.project_path}Terralistic-server.app/ -exportOptionsPlist {self.project_path}Terralistic.xcodeproj/exportOptions.plist")
        utils.copy(f"{self.project_path}Terralistic-server.app/Terralistic-server.app/", f"{self.project_path}Output/MacOS/Terralistic-server.app/")
        utils.remove(self.project_path + "Terralistic-server.xcarchive/")
        utils.remove(self.project_path + "Terralistic-server.app/")


def buildForWindows(project_path: str, arg: str):
    """

    utils.createDir(project_path + "Output/Windows/Terralistic")
    utils.copy(f"{project_path}Build/Terralistic.exe", f"{project_path}Output/Windows/Terralistic/Terralistic.exe")

    utils.createDir(project_path + "Output/Windows/Terralistic-server/")
    utils.copy(f"{project_path}Build/Terralistic-server.exe", f"{project_path}Output/Windows/Terralistic-server/Terralistic-server.exe")

    for file in utils.listDir(project_path + "Build/"):
        if file.endswith(".dll"):
            utils.copy(f"{project_path}Build/{file}", f"{project_path}Output/Windows/Terralistic/")
            utils.copy(f"{project_path}Build/{file}", f"{project_path}Output/Windows/Terralistic-server/")

    utils.copy(project_path + "Dependencies/patch-2.5.9-7-bin/bin/patch.exe", f"{project_path}Output/Windows/Terralistic/patch.exe")

    if utils.exists(f"{project_path}Output/Windows/Terralistic/Resources/"):
        utils.remove(f"{project_path}Output/Windows/Terralistic/Resources/")
    utils.move(f"{project_path}Build/Resources/", f"{project_path}Output/Windows/Terralistic/Resources/")
    utils.copy(f"{project_path}Resources/resourcePack/misc/Structures.asset", f"{project_path}Output/Windows/Terralistic-server/Structures.asset")
    utils.copy(f"{project_path}Resources/font.opa", f"{project_path}Output/Windows/Terralistic-server/font.opa")

    if arg == "run":
        utils.system(f"\"{project_path}Output/Windows/Terralistic/Terralistic.exe\"")

    """
    task_manager = tasks.TaskManager()

    task_manager.registerTask(CreateDirs(project_path))
    task_manager.registerTask(InstallZlib(project_path))
    task_manager.registerTask(InstallGlfw(project_path))
    task_manager.registerTask(GenerateGlad(project_path))
    task_manager.registerTask(InstallPatch(project_path))
    task_manager.registerTask(InstallPlatformFolders(project_path))
    task_manager.registerTask(InstallPerlinNoise(project_path))
    task_manager.registerTask(CompileResourcePack(project_path))
    if arg != "nobuild":
        task_manager.registerTask(Build(project_path))
        #task_manager.registerTask(BuildServer(project_path))
        #task_manager.registerTask(UnpackClient(project_path))
        #task_manager.registerTask(UnpackServer(project_path))

    task_manager.run()