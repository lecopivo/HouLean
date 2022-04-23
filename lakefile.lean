import Lake
open Lake DSL

package HouLean {
  defaultFacet := PackageFacet.sharedLib
  -- moreLinkArgs := #["-L/home/tomass/.elan/toolchains/leanprover--lean4---nightly-2022-01-12/lib/lean", "-lleanshared",
  --                   "-L/home/tomass/houdini19.0/dso", "-lHouLeanCore"],
  libName := "libHouLean"
  dependencies := #[
    {name := `SciLean
     src := Source.git "https://github.com/lecopivo/SciLean.git" "40b117bbe839f8fcf07dc851906a04156489de4a"
    }]
}

script lean_sysroot (args) do
  IO.println (← getLeanSysroot)
  return 0

script compile (args) do
  -- let wrangleName := args.getD 0 ""
  let wrangleDir := System.FilePath.mk (args.getD 0 "")
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
      -- "-I", (← getLeanIncludeDir).toString, 
      "-shared",
      "-L", ((← IO.currentDir) / defaultBuildDir / defaultLibDir).toString, "-lHouLean",
      "-lleanshared", "-lInit", "-lLean",
      "-L/home/tomass/houdini19.0/dso", "-lHouLeanCore",
      "-o", wrangleLibFile.toString]
  }
  if generateBinary.exitCode ≠ 0 then
    IO.eprintln generateBinary.stderr
    IO.eprintln generateBinary.stdout
    return generateBinary.exitCode

  return 0

