import Lake
open Lake DSL

package HouLean {
  defaultFacet := PackageFacet.sharedLib
  -- moreLinkArgs := #["-L/home/tomass/.elan/toolchains/leanprover--lean4---nightly-2022-01-12/lib/lean", "-lleanshared",
  --                   "-L/home/tomass/houdini19.0/dso", "-lHouLeanCore"],
  libName := "libHouLean"
  dependencies := #[
    {name := `SciLean
     src := Source.git "https://github.com/lecopivo/SciLean.git" "2600a9540cbd1f09702c3c5bac9bf86bd2f0a3c5"
    }]
}
