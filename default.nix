{ lib
, llvmPackages_11
, cmake
, spdlog
, abseil-cpp }:

llvmPackages_11.stdenv.mkDerivation rec {
  pname = "rummibuk-helper";
  version = "1.0.0";

  src = ./.;

  nativeBuildInputs = [ cmake ];
  buildInputs = [ spdlog abseil-cpp ];

  cmakeFlags = [
    "-DENABLE_TESTING=OFF"
    "-DENABLE_INSTALL=ON"
  ];

  meta = with lib; {
    homepage = "https://github.com/breakds/rummibuk-helper";
    description = ''
      Solver for the game Rummibuk.
    '';
    licencse = licenses.mit;
    platforms = with platforms; linux ++ darwin;
    maintainers = [ maintainers.breakds ];
  };
}
