import Lake
open Lake DSL

package HouLean {
  defaultFacet := PackageFacet.sharedLib
  libName := "libHouLean"
  moreLeancArgs := #["-O2", "-Wall", "-DNDEBUG"]
}

script compileCpp (args) do

  let hfs := System.FilePath.mk (args.getD 0 "")

  -- make build directory
  let makeBuildDir ← IO.Process.run {
    cmd := "mkdir"
    args := #["-p", "build/cpp"]
  }

  -- run cmake
  if ¬(← defaultBuildDir / "cpp" / "Makefile" |>.pathExists) then
    let runCMake ← IO.Process.spawn {
      cmd := "cmake"
      args := #["../../cpp", 
                "-DCMAKE_EXPORT_COMPILE_COMMANDS=1",
                "-DCMAKE_BUILD_TYPE=Release",
                s!"-DCMAKE_HFS={hfs}",
                s!"-DCMAKE_LEAN_SYSROOT={← getLeanSysroot}"]
      cwd := defaultBuildDir / "cpp" |>.toString      
    }
    let out ← runCMake.wait
    if out != 0 then
      return out

  let runMake ← IO.Process.spawn {
    cmd := "make"
    args := #["-j"]
    cwd := defaultBuildDir / "cpp" |>.toString
  }
  let out ← runMake.wait
  if out != 0 then
    return out 

  return 0


script install (args) do

  let houUserPrefDir := System.FilePath.mk (args.getD 0 "")
  let dsoDir := houUserPrefDir / "dso"
  let otlDir := houUserPrefDir / "otls"
  
  -- Link libHouLean to dso directory
  let linkLib ← IO.Process.run {
    cmd := "ln"
    args := #["-sf", 
              (← IO.currentDir) / "build" / "lib" / "libHouLean.so" |>.toString,
              "libHouLean.so"]
    cwd := dsoDir
  }
  let linkOtl ← IO.Process.run {
    cmd := "ln"
    args := #["-sf", 
              (← IO.currentDir) / "houdini" / "otl" / "sop_tomass.dev.lean.1.0.hda" |>.toString,
              "sop_tomass.dev.lean.1.0.hda"]
    cwd := otlDir
  }

  return 0
