import Lake
open Lake DSL

package houlean

@[default_target]
lean_lib HouLean {
  roots := #[`HouLean]
  buildType := .release
}

require scilean from git "https://github.com/lecopivo/SciLean" @ "master"

-- can I get this automatically with lake somehow? 
def depPckgs : Array (String × String) := 
  #[("aesop", "Aesop"),
    ("Cli", "Cli"),
    ("importGraph", "ImportGraph"),
    ("leancolls", "LeanColls"),
    ("mathlib", "Mathlib"),
    ("proofwidgets", "ProofWidgets"),
    ("Qq", "Qq"),
    ("scilean", "SciLean"),
    ("std", "Std")]


def depPckgsLinkerFlags := #["-L./build/cpp", "-lHouLeanCApi"]
    -- depPckgs 
    --   |>.map (fun (dir,name) => #[s!"-L.lake/packages/{dir}/.lake/build/lib", s!"-l{name}"])
    --   |>.foldl (init:=#[]) (·++·)
    --   |>.push "-lLake"
    --   |>.push "-L./build/cpp"
    --   |>.push "-lHouLeanCApi"

open Lean Elab Command in
/-- This command creates a new target for each directory in `Wrangles/`. 

To build specific wrangle code you most likely want to run `lake build <dir_name>:shared` -/
def generate_wrangle_targets : CommandElabM Unit := do
  -- Directory Wrangles might not exist! Do not fail compilation
  
  let dirName := "Wrangles"
  let dir := (← IO.currentDir) / dirName

  for p in ← dir.readDir do
    if ← p.path.isDir then
      let libName := p.path.fileName.get!
      let name := mkIdent libName
      let root := Syntax.mkStrLit (dirName ++ "/" ++ libName ++ "/Main" )
      IO.println s!"found wrangle {libName}"
      elabCommand (← 
        `(lean_lib $name:ident { 
            roots := #[$root,`HouLean]
            defaultFacets := #[`shared]            
            buildType := .release
            moreLinkArgs := depPckgsLinkerFlags }))
    
#eval generate_wrangle_targets


script compileCpp (args) do

  let hfs := System.FilePath.mk (args.getD 0 "")

  let buildDir := (← IO.currentDir) / "build" / "cpp"

  -- make build directory
  let _ ← IO.Process.run {
    cmd := "mkdir"
    args := #["-p", buildDir.toString]
  }

  -- run cmake
  if ¬(← buildDir / "Makefile" |>.pathExists) then
    let runCMake ← IO.Process.spawn {
      cmd := "cmake"
      args := #["../../cpp", 
                "-DCMAKE_EXPORT_COMPILE_COMMANDS=1",
                "-DCMAKE_BUILD_TYPE=Release",
                s!"-DCMAKE_HFS={hfs}",
                s!"-DCMAKE_LEAN_SYSROOT={← getLeanSysroot}"]
      cwd := buildDir
    }
    let out ← runCMake.wait
    if out != 0 then
      return out

  let runMake ← IO.Process.spawn {
    cmd := "make"
    args := #["-j"]
    cwd := buildDir
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
  unless ← houUserPrefDir / "houdini.env" |>.pathExists do
    IO.println s!"invalid houdini preference directory {houUserPrefDir}"
    return 1

  -- let dsoDir := houUserPrefDir / "dso"
  let libDir := houUserPrefDir / "lib"
  let otlDir := houUserPrefDir / "otls"

  -- make lib directory
  let _ ← IO.Process.run {
    cmd := "mkdir"
    args := #["-p", libDir.toString]
  }
  
  -- link Lib
  -- link libleanshared
  let _ ← IO.Process.run {
    cmd := "ln"
    args := #["-sf", 
              (← getLeanLibDir) / "libleanshared.so" |>.toString,
              "libleanshared.so"]
    cwd := libDir
  }
  -- link libLake
  let _ ← IO.Process.run {
    cmd := "ln"
    args := #["-sf", 
              (← IO.currentDir) / "build" / "libLake.so" |>.toString,
              "libLake.so"]
    cwd := libDir
  }
  -- link all dependencies
  for (dir,name) in depPckgs do
    let _ ← IO.Process.run {
      cmd := "ln"
      args := #["-sf", 
                (← IO.currentDir) / ".lake" / "packages" / dir / ".lake" / "build" / "lib" / s!"lib{name}.so" |>.toString,
                s!"lib{name}.so"]
      cwd := libDir
    }
    

  -- TODO: ensruse that folderleanshared: houUserPrefDir / "otl" exists!

  -- link otl
  let _ ← IO.Process.run {
    cmd := "ln"
    args := #["-sf", 
              (← IO.currentDir) / "houdini" / "otl" / "sop_tomass.dev.lean.2.0.hda" |>.toString,
              "sop_tomass.dev.lean.2.0.hda"]
    cwd := otlDir
  }

  return 0
