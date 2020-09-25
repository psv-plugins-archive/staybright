# This file is part of Staybright
# Copyright © 2020 浅倉麗子
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, version 3 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.13)

set(CMAKE_TOOLCHAIN_FILE "$ENV{DOLCESDK}/share/dolce.toolchain.cmake" CACHE PATH "DolceSDK toolchain file")
set(DOLCESDK_CMAKE_FILE "$ENV{DOLCESDK}/share/dolce.cmake" CACHE PATH "DolceSDK CMake functions file")
include("${DOLCESDK_CMAKE_FILE}" REQUIRED)

project(staybright LANGUAGES C)

add_compile_options(
  -Os -std=c99
  -Wall -Wextra -Wdouble-promotion -Wshadow -Wundef
  -fsingle-precision-constant -fno-common -flto
  -ftree-vectorize -funsafe-math-optimizations
)

add_link_options(
  -nostdlib
  --entry=module_start
)

# Build self

set(ELF "staybright.elf")
set(SELF "staybright.skprx")
set(MODULE_INFO "staybright.yml")

add_executable("${ELF}"
  staybright.c
)

target_link_libraries("${ELF}"
  taihenForKernel_stub
  ScePowerForDriver_stub
)

dolce_create_self("${SELF}"
  "${ELF}"
  CONFIG "${MODULE_INFO}"
)
