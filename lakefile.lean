import Lake
open Lake DSL

package houlean

@[defaultTarget]
lean_lib HouLean {
  roots := #[`HouLean]
  buildType := .release
}

-- require scilean from git "https://github.com/lecopivo/SciLean" @ "master"

open Lean Elab Command in
/-- This command creates a new target for each directory in `Wrangles/`. 

To build specific wrangle code you most likely want to run `lake build <dir_name>:shared` -/
def generate_wrangle_targets : CommandElabM Unit := do
  let dirName := "Wrangles"
  let dir := (← IO.currentDir) / dirName

  for p in ← dir.readDir do
    if ← p.path.isDir then
      let libName := p.path.fileName.get!
      let name := mkIdent libName
      let root := Syntax.mkStrLit (dirName ++ "/" ++ libName ++ "/Main.lean" )
      elabCommand (← 
        `(lean_lib $name:ident { 
            roots := #[$root,`HouLean]
            buildType := .release
            moreLinkArgs := #["-L./build/cpp", "-lHouLeanCApi"] }))
    
#eval generate_wrangle_targets


script compileCpp (args) do

  let hfs := System.FilePath.mk (args.getD 0 "")

  -- make build directory
  let _ ← IO.Process.run {
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

script setHoudiniEnv (args) do

  let houUserPrefDir := System.FilePath.mk (args.getD 0 "")
  let houdiniEnvFile := houUserPrefDir / "houdini.env"

  let curDir ← IO.currentDir
  let newEnv := s!"HOULEAN_DIR = {curDir}"

  let _ ← IO.FS.withFile houdiniEnvFile IO.FS.Mode.readWrite λ file => do
    let lines := (← file.readToEnd).splitOn "\n"
    let isHouLeanEnvSet? := lines.find? (λ line => line.substrEq 0 newEnv 0 11)
    match isHouLeanEnvSet? with
    | some str => 
      IO.println s!"HouLean environment variable is already set in {houdiniEnvFile}!\n{str}"
    | none => 
      file.putStrLn newEnv
      IO.println s!"Setting HouLean environment variable in {houdiniEnvFile}\n{newEnv}"

  return 0
  

script install (args) do

  let houUserPrefDir := System.FilePath.mk (args.getD 0 "")
  -- let dsoDir := houUserPrefDir / "dso"
  let libDir := houUserPrefDir / "lib"
  let otlDir := houUserPrefDir / "otls"

  -- make lib directory
  let _ ← IO.Process.run {
    cmd := "mkdir"
    args := #["-p", libDir.toString]
  }
  
  -- link Lib
  let _ ← IO.Process.run {
    cmd := "ln"
    args := #["-sf", 
              (← getLeanLibDir) / "libleanshared.so" |>.toString,
              "libleanshared.so"]
    cwd := libDir
  }

  -- link otl
  let _ ← IO.Process.run {
    cmd := "ln"
    args := #["-sf", 
              (← IO.currentDir) / "houdini" / "otl" / "sop_tomass.dev.lean.2.0.hda" |>.toString,
              "sop_tomass.dev.lean.2.0.hda"]
    cwd := otlDir
  }

  return 0
