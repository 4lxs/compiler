{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    flake-parts.url = "github:hercules-ci/flake-parts";
    miniCompileCommands = {
      url = "github:danielbarter/mini_compile_commands/v0.6";
      flake = false;
    };
    koturNixPkgs = {
      url = "github:nkoturovic/kotur-nixpkgs/v0.6.0";
      flake = false;
    };
    systems.url = "github:nix-systems/default";
  };
  outputs = {
    self,
    flake-parts,
    systems,
    miniCompileCommands,
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
        mcc-env = (pkgs.callPackage miniCompileCommands {}).wrap pkgs.stdenv;
        mcc-hook = (pkgs.callPackage miniCompileCommands {}).hook;
        build-packages = [
          pkgs.cmake
          pkgs.ninja
          # pkgs.clang_18
          pkgs.gcc14
        ];
      in {
        packages.default = mcc-env.mkDerivation (self: {
          name = "llvm";
          version = "0.0.1";

          nativeBuildInputs = [mcc-hook] ++ build-packages;

          src = builtins.path {
            path = ./.;
            filter = path: type:
              !(pkgs.lib.hasPrefix "." (baseNameOf path));
          };

          cmakeFlags = [
            "--no-warn-unused-cli"
          ];
        });
        devShells.default = (pkgs.mkShell.override {stdenv = mcc-env;}) {
          buildInputs = [
            config.formatter

            pkgs.clang-tools
            pkgs.cmake-lint
            pkgs.cmake-format
            pkgs.doxygen
            pkgs.ccache
            pkgs.cppcheck
            pkgs.include-what-you-use
              pkgs.cmakeCurses
          ] ++ build-packages;
        };
        formatter = pkgs.alejandra;
      };
    };
}
