add_rules("mode.debug", "mode.release")
--add_repositories("xavine-xrepo https://github.com/FranekStratovarius/xmake-repo dev")
--add_repositories("xavine-xrepo testrepo")
if is_plat("macosx") then
	-- use static libs on macosx
	add_requires("bgfx 7816", "flecs v3.0.0", "glfw 3.3.8", "glm 0.9.9+8", {system = false})
else
	add_requires("bgfx 7816", "flecs v3.0.0", "glfw 3.3.8", "glm 0.9.9+8", {system = false, configs = {shared = true}})
end

rule("shader") do
	set_extensions(".sc")
	on_buildcmd_file(function (target, batchcmds, sourcefile, opt)
		-- list of shader models
		local shader_models = {
			-- OpenGL ES Shading Language / WebGL (ESSL)
			{lang = "essl", id = "320_es"},
			-- Metal Shading Language (MSL)
			{lang = "metal", id = "metal"},
			-- Standard Portable Intermediate Representation - V (SPIR-V) [Vulkan]
			{lang = "spirv", id = "spirv15-12"},
			-- OpenGL Shading Language (GLSL)
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
			-- make sure build directory exists
			batchcmds:mkdir(path.join(target:targetdir(), "assets", "shaders", shader_model.lang))

			-- get type of shader
			local shader_type = string.sub(path.basename(sourcefile), 1, 2)
			local shader_tags = nil
			if shader_type == "fs" then
				shader_tags = {long = "fragment", short = "p"}
			elseif shader_type == "vs" then
				shader_tags = {long = "vertex  ", short = "v"}
			elseif shader_type == "cs" then
				shader_tags = {long = "compute ", short = "c"}
			else
				return
			end

			-- compile shader
			local targetfile = path.join(target:targetdir(), "assets", "shaders", shader_model.lang, path.basename(sourcefile)..".bin")
			batchcmds:vrunv(
				path.join(target:pkgs()["bgfx"]:installdir(), "bin", "shadercRelease")
				.." -f "..sourcefile
				.." -o "..targetfile
				..vformat(" --platform $(os)")
				.." --profile "..(shader_model.dx and shader_tags.short or "")..shader_model.id
				.." --type "..shader_tags.long
				..vformat(" --varyingdef "..path.join("$(projectdir)", "shaders", "varying.def.sc"))
				--..vformat(" --verbose")
				..vformat(" -i "..path.join("$(projectdir)", "shaders", "src"))
			)
			batchcmds:show_progress(opt.progress, "${color.build.object}compiling "..shader_tags.long.." shader %s to %s", sourcefile, targetfile)
		end	-- shader_model

		-- only rebuild the file if its changed since last run
		batchcmds:add_depfiles(sourcefile)
    end)
end

target("xavine") do
	set_kind("binary")
	if not is_plat("windows") then
		set_languages("cxx11")
	end

	set_warnings("all")

	if is_mode("debug") then
		-- add macro: DEBUG
		add_defines("DEBUG")
		-- enable debug symbols
		set_symbols("debug")
		-- disable optimization
		set_optimize("none")
	elseif is_mode("release") then
		-- mark symbols visibility as hidden
		set_symbols("hidden")
		-- strip all symbols
		set_strip("all")
		-- enable optimization
		set_optimize("fastest")
		-- fomit frame pointer
		add_cxflags("-fomit-frame-pointer")
		add_mxflags("-fomit-frame-pointer")
	end

	add_files("src/**.cpp")
	add_includedirs("include",{public=true})

	-- set bgfx platform defines
	if is_plat("linux") then
		add_syslinks("dl", "pthread")
		add_defines("BX_PLATFORM_LINUX")
	elseif is_plat("windows") then
		add_defines("BX_PLATFORM_WINDOWS")
	elseif is_plat("macosx") then
		add_defines("BX_PLATFORM_OSX")
	end
	-- set bgfx to multithreaded
	add_defines("BGFX_CONFIG_MULTITHREADED=1")
	-- enable experimental features fro glm https://github.com/g-truc/glm/blob/master/manual.md#-74-should-i-use-gtx-extensions
	add_defines("GLM_ENABLE_EXPERIMENTAL")

	add_rules("shader")
	add_files("shaders/**.sc")

	if is_plat("macosx") then
		-- link statically on macosx
		add_packages("bgfx", "flecs", "glfw", "glm")
	else
		add_packages("bgfx", (not is_plat("windows")) and {links="bgfx-shared-libRelease"} or nil)
		add_packages("flecs", {links = "flecs"})
		add_packages("glfw", {links = is_plat("windows") and "glfw3dll" or "glfw"})
		add_packages("glm")
		-- add folder of executable to LD_LIBRARY_PATH
		add_rpathdirs(".")
	end

	-- copy asset folder after build
	after_build(function (target)
		-- copy assets to build output folder
		os.cp(path.join("assets"), path.join("$(buildir)", "$(os)", "$(arch)", "$(mode)"))
		-- copy dynamic libs to build output folder
		if not is_plat("macosx") then
			-- only copy dynamic libs if not in macosx
			for _, package in ipairs({
				{name = "bgfx", libname = "bgfx-shared-libRelease"},
				{name = "flecs", libname = "flecs"},
				{name = "glfw", libname = is_plat("windows") and "glfw3" or "glfw"}
			}) do
				os.cp(
					path.join(target:pkgs()[package.name]:installdir(), (is_plat("windows") and "bin" or "lib"), "*"..package.libname..(is_plat("windows") and ".dll" or ".so")),
					path.join("$(buildir)", "$(os)", "$(arch)", "$(mode)")
				)
			end
		end
	end)

	--[[
	on_package(function (target)
		os.cp(path.join("assets"), path.join("$(buildir)", "packages", "x", "xavine", "$(os)", "$(arch)", "$(mode)", "bin"))
	end)
	--]]
end