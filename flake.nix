{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    flake-parts.url = "github:hercules-ci/flake-parts";
    systems.url = "github:nix-systems/default";
  };
  outputs = {
    self,
    flake-parts,
    systems,
    ...
  } @ inputs:
    flake-parts.lib.mkFlake {inherit inputs;} {
      systems = import systems;
      flake = {
      };
      perSystem = {
        pkgs,
        system,
        config,
        ...
      }: let
        llvm = pkgs.llvmPackages_19;
      in {
        devShells.default = (pkgs.mkShell.override {inherit (llvm) stdenv;}) {
          nativeBuildInputs = [
            pkgs.cmake
            pkgs.ninja

            # clang-tools must be above stdlib because ... reasons
            # https://discourse.nixos.org/t/clang-tidy-doesnt-find-stdlib/37641/3
            llvm.clang-tools
            llvm.libcxx
            llvm.libllvm
          ];

          buildInputs = [
            config.formatter

            pkgs.cmake-lint
            pkgs.cmake-format
            pkgs.doxygen
            pkgs.ccache
            pkgs.cppcheck
            pkgs.include-what-you-use
            pkgs.cmakeCurses
          ];
          LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath [llvm.libstdcxxClang];
        };
        formatter = pkgs.alejandra;
      };
    };
}
