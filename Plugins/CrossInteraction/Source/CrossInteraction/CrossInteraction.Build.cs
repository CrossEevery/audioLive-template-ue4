// Some copyright should be here...

using UnrealBuildTool;
using System.IO;




public class CrossInteraction : ModuleRules
{


	public CrossInteraction(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				"CrossInteraction/Public"
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"CrossInteraction/Private"
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", "CoreUObject", "Engine", "InputCore", "Sockets", "Networking", "Messaging", "RenderCore", "RHI"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"InputCore",
				"Json",
				"JsonUtilities",
				"Networking",
				"Sockets",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);



		LoadOpenCV(Target);
	}
	
	private string ThirdPartyDirectory
	{ get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../ThirdParty")); } }

	public bool LoadOpenCV(ReadOnlyTargetRules Target)
	{
		//获得OpenCV第三方库的根路径
		string OpenCVPath = Path.Combine(ThirdPartyDirectory, "OpenCV");
		//Lib文件路径
		string LibPath = "";
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			//引擎将会include该文件夹下的第三方库，如.hpp文件.
			PublicIncludePaths.AddRange(new string[] { Path.Combine(OpenCVPath, "Includes") });

			//指向ThirdpartyPath/Libraries/Win64/
			LibPath = Path.Combine(OpenCVPath, "Libraries", "Win64");

			//这将告诉引擎lib文件的路径
			PublicSystemLibraryPaths.Add(LibPath);
			//这将告诉引擎需要加载的lib文件的名称，他将去上面的lib文件路径下寻找我们提供的lib文件
			PublicAdditionalLibraries.Add("opencv_world453.lib");

			/**
			 * dll需要拷贝到项目根目录/Binaries/当前平台/路径下
			 *运行时依赖，可以在打包时，自动将dll从源文件路径拷贝到慕白哦文件路径
			 * RuntimeDependencies.Add(源文件路径，目标文件路径）
			 * $(BinaryOutputDir),表示Binaries/当前平台/路径
			 * 参考：https://docs.unrealengine.com/4.27/zh-CN/ProductionPipelines/BuildTools/UnrealBuildTool/ThirdPartyLibraries/
			 */
			RuntimeDependencies.Add(Path.Combine("$(BinaryOutputDir)", "opencv_world453.dll"), Path.Combine(LibPath, "opencv_world453.dll"));
			RuntimeDependencies.Add(Path.Combine("$(BinaryOutputDir)", "opencv_videoio_ffmpeg453_64.dll"), Path.Combine(LibPath, "opencv_videoio_ffmpeg453_64.dll"));
			return true;
		}
		return false;
	}
}
