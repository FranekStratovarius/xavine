add_rules("mode.debug", "mode.release")
add_repositories("xavine-xrepo https://github.com/FranekStratovarius/xmake-repo master")
add_requires("bgfx 7816", "flecs v3.0.1-alpha", "glfw 3.3.5", {system = false})
add_requireconfs("bgfx")
add_requireconfs("flecs", {configs = {shared = true}})
add_requireconfs("glfw", {configs = {shared = true}})

target("xavine") do
	set_kind("binary")
	set_languages("cxx11")

	add_files("src/**.cpp")
	add_includedirs("include",{public=true})

	set_warnings("all")
	set_optimize("fastest")

	-- set bgfx platform defines
	if is_plat("linux") then
		add_syslinks("dl")
		add_defines("BX_PLATFORM_LINUX")
	elseif is_plat("windows") then
		add_defines("BX_PLATFORM_WINDOWS")
	elseif is_plat("macosx") then
		add_defines("BX_PLATFORM_OSX")
	end

	add_packages("bgfx", "flecs", "glfw")

	-- copy asset folder after build
	after_build(function (target)
		-- copy assets to build output folder
		os.cp(path.join("assets"), path.join("$(buildir)", "$(os)", "$(arch)", "$(mode)"))
		
		-- list of shader models
		-- shadercompilierung auf custom rule umbauen: https://xmake.io/#/manual/custom_rule
		local shader_models = {
			-- OpenGL ES Shading Language / WebGL (ESSL)
			{lang = "essl", id = "320_es"},

			-- Metal Shading Language (MSL)
			{lang = "metal", id = "metal"},
			
			-- Standard Portable Intermediate Representation - V (SPIR-V) [Vulkan]
			{lang = "spirv", id = "spirv15-12"},

			-- OpenGL Shading Language (GLSL)
			--{lang = "glsl", id = 150},
			{lang = "glsl", id = "440"},
		}

		-- High-Level Shading Language (HLSL) [DirectX] only on windows
		if is_plat("windows") then
			table.insert(shader_models, {lang = "dx9",  id = "s_3_0", dx = true})
			table.insert(shader_models, {lang = "dx11", id = "s_4_0", dx = true})
			table.insert(shader_models, {lang = "dx12", id = "s_5_0", dx = true})
		end

		-- compile shaders and output them in the build output folder
		for _, shader_model in ipairs(shader_models) do
			-- create output folder
			os.mkdir(vformat(path.join("$(buildir)", "$(os)", "$(arch)", "$(mode)", "assets", "shaders", shader_model.lang)))
			-- compile fragment and vertex shader
			for _, shader_type in ipairs({
				{long = "fragment", short = "fs", shorter = "p"},
				{long = "vertex", short = "vs", shorter = "v"},
				{long = "compute", short = "cs", shorter = "c"},
			}) do
				-- compile multiple shaders
				for _, file_path in ipairs(os.files(path.join("$(projectdir)", "shaders", shader_type.short.."_*.sc"))) do
					os.exec(
						path.join(target:pkgs()["bgfx"]:installdir(), "bin", "shadercRelease")
						..vformat(" -f "..file_path)
						..vformat(" -o "..path.join("$(buildir)", "$(os)", "$(arch)", "$(mode)", "assets", "shaders", shader_model.lang, path.basename(file_path)..".bin"))
						..vformat(" --platform $(os)")
						..vformat(" --profile "..(shader_model.dx and shader_model.shorter or "")..shader_model.id)
						..vformat(" --type "..shader_type.long)
						..vformat(" --varyingdef "..path.join("$(projectdir)", "shaders", "varying.def.sc"))
						--..vformat(" --verbose")
						..vformat(" -i "..path.join("$(projectdir)", "shaders", "src"))
					)
				end	-- file_path
			end	-- shader_type
		end	-- shader_model
	end)

	--[[
	on_package(function (target)
		os.cp(path.join("assets"), path.join("$(buildir)", "packages", "x", "xavine", "$(os)", "$(arch)", "$(mode)", "bin"))
	end)
	--]]
end