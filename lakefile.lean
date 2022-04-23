import Lake
open Lake DSL

package HouLean {
  defaultFacet := PackageFacet.sharedLib
  -- moreLinkArgs := #["-L/home/tomass/.elan/toolchains/leanprover--lean4---nightly-2022-01-12/lib/lean", "-lleanshared",
  --                   "-L/home/tomass/houdini19.0/dso", "-lHouLeanCore"],
  libName := "libHouLean"
  moreLeancArgs := #["-O3", "-Wall", "-DNDEBUG"]
  -- dependencies := #[
  --   {name := `SciLean
  --    src := Source.git "https://github.com/lecopivo/SciLean.git" "40b117bbe839f8fcf07dc851906a04156489de4a"
  --   }]
}

-- Returns Lean install directory
script lean_sysroot (args) do
  IO.println (← getLeanSysroot)
  return 0

script build (args) do
  let houUserPrefDir := System.FilePath.mk (args.getD 0 "")
  let dsoDir := houUserPrefDir / "dso"
  let otlDir := houUserPrefDir / "otls"

  if ¬(← (houUserPrefDir / "houdini.env").pathExists) then
    IO.eprintln s!"Invalid houdini preference directory! Could not find {houUserPrefDir / "houdini.env"}"
    return 1

  -- Build Lean part of the library
  IO.println "Building HouLean"
  let buildHouLean ← IO.Process.spawn {
    cmd := "lake"
    args := #["build"]
  }
  let out ← buildHouLean.wait
  if out != 0 then
    return out 

  IO.println "Building HouLean Houdini plugin"
  -- make build directory
  let buildHouLeanCoreStep1 ← IO.Process.run {
    cmd := "mkdir"
    args := #["-p", "build/release"]
    cwd := (← IO.currentDir) / "cpp" |>.toString
  }
  -- run cmake
  if ¬(← (← IO.currentDir) / "cpp" / "build" / "release" / "Makefile" |>.pathExists) then
    let buildHouLeanCoreStep2 ← IO.Process.spawn {
      cmd := "cmake"
      args := #["../..", "-DCMAKE_EXPORT_COMPILE_COMMANDS=1", "-DCMAKE_BUILD_TYPE=Release"]
      cwd := (← IO.currentDir) / "cpp" / "build" / "release" |>.toString
    }
    let out ← buildHouLeanCoreStep2.wait
    if out != 0 then
      return out 

  -- run make
  let buildHouLeanCoreStep3 ← IO.Process.spawn {
    cmd := "make"
    args := #["-j"]
    cwd := (← IO.currentDir) / "cpp" / "build" / "release" |>.toString
  }
  let out ← buildHouLeanCoreStep3.wait
  if out != 0 then
    return out 


  -- Link libHouLean to dso directory
  let copyLib ← IO.Process.run {
    cmd := "ln"
    args := #["-sf", 
              (← IO.currentDir) / "build" / "lib" / "libHouLean.so" |>.toString,
              "libHouLean.so"]
    cwd := dsoDir
  }
  let copyOtl ← IO.Process.run {
    cmd := "ln"
    args := #["-sf", 
              (← IO.currentDir) / "houdini" / "otl" / "sop_tomass.dev.leanwrangle.1.0.hda" |>.toString,
              "sop_tomass.dev.leanwrangle.1.0.hda"]
    cwd := otlDir
  }

  return 0

script compile (args) do
  let houUserPrefDir := System.FilePath.mk (args.getD 0 "")
  let dsoDir := houUserPrefDir / "dso"

  let wrangleDir := System.FilePath.mk (args.getD 1 "")
  let wrangleName := wrangleDir.fileName.getD ""
  -- let wrangleDir := (← IO.currentDir) / "wrangles" / wrangleName
  let wrangleLeanFile := wrangleDir / "Main.lean"

  if ¬(←wrangleLeanFile.pathExists) then
    IO.eprintln s!"Wrangle lean file {wrangleLeanFile} not found!"
    return 1

  let wrangleCFile := wrangleDir / s!"{wrangleName}.c"
  let generateCCode ← IO.Process.output {
    cmd := "lake"
    args := #["env", "lean", wrangleLeanFile.toString, 
              s!"--root={wrangleDir.toString}",
              "-c", wrangleCFile.toString]
  }
  if generateCCode.exitCode ≠ 0 then
    IO.eprintln generateCCode.stderr
    IO.eprintln generateCCode.stdout
    return generateCCode.exitCode

  let wrangleLibFile := wrangleDir / "Main.so"
  let generateBinary ← IO.Process.output {
    cmd := "leanc"
    args := #[wrangleCFile.toString, 
      "-O3", "-Wall", "-DNDEBUG", "-shared",
      s!"-L{dsoDir}", "-lHouLeanCore", "-lHouLean", "-lleanshared",
      "-o", wrangleLibFile.toString]
  }
  if generateBinary.exitCode ≠ 0 then
    IO.eprintln generateBinary.stderr
    IO.eprintln generateBinary.stdout
    return generateBinary.exitCode

  return 0

