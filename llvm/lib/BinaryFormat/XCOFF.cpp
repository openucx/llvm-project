//===-- llvm/BinaryFormat/XCOFF.cpp - The XCOFF file format -----*- C++/-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/BinaryFormat/XCOFF.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringRef.h"

using namespace llvm;

#define SMC_CASE(A)                                                            \
  case XCOFF::XMC_##A:                                                         \
    return #A;
StringRef XCOFF::getMappingClassString(XCOFF::StorageMappingClass SMC) {
  switch (SMC) {
    SMC_CASE(PR)
    SMC_CASE(RO)
    SMC_CASE(DB)
    SMC_CASE(GL)
    SMC_CASE(XO)
    SMC_CASE(SV)
    SMC_CASE(SV64)
    SMC_CASE(SV3264)
    SMC_CASE(TI)
    SMC_CASE(TB)
    SMC_CASE(RW)
    SMC_CASE(TC0)
    SMC_CASE(TC)
    SMC_CASE(TD)
    SMC_CASE(DS)
    SMC_CASE(UA)
    SMC_CASE(BS)
    SMC_CASE(UC)
    SMC_CASE(TL)
    SMC_CASE(UL)
    SMC_CASE(TE)
#undef SMC_CASE
  }

  // TODO: need to add a test case for "Unknown" and other SMC.
  return "Unknown";
}

#define RELOC_CASE(A)                                                          \
  case XCOFF::A:                                                               \
    return #A;
StringRef XCOFF::getRelocationTypeString(XCOFF::RelocationType Type) {
  switch (Type) {
    RELOC_CASE(R_POS)
    RELOC_CASE(R_RL)
    RELOC_CASE(R_RLA)
    RELOC_CASE(R_NEG)
    RELOC_CASE(R_REL)
    RELOC_CASE(R_TOC)
    RELOC_CASE(R_TRL)
    RELOC_CASE(R_TRLA)
    RELOC_CASE(R_GL)
    RELOC_CASE(R_TCL)
    RELOC_CASE(R_REF)
    RELOC_CASE(R_BA)
    RELOC_CASE(R_BR)
    RELOC_CASE(R_RBA)
    RELOC_CASE(R_RBR)
    RELOC_CASE(R_TLS)
    RELOC_CASE(R_TLS_IE)
    RELOC_CASE(R_TLS_LD)
    RELOC_CASE(R_TLS_LE)
    RELOC_CASE(R_TLSM)
    RELOC_CASE(R_TLSML)
    RELOC_CASE(R_TOCU)
    RELOC_CASE(R_TOCL)
  }
  return "Unknown";
}

#define LANG_CASE(A)                                                           \
  case XCOFF::TracebackTable::A:                                               \
    return #A;

StringRef XCOFF::getNameForTracebackTableLanguageId(
    XCOFF::TracebackTable::LanguageID LangId) {
  switch (LangId) {
    LANG_CASE(C)
    LANG_CASE(Fortran)
    LANG_CASE(Pascal)
    LANG_CASE(Ada)
    LANG_CASE(PL1)
    LANG_CASE(Basic)
    LANG_CASE(Lisp)
    LANG_CASE(Cobol)
    LANG_CASE(Modula2)
    LANG_CASE(Rpg)
    LANG_CASE(PL8)
    LANG_CASE(Assembly)
    LANG_CASE(Java)
    LANG_CASE(ObjectiveC)
    LANG_CASE(CPlusPlus)
  }
  return "Unknown";
}
#undef LANG_CASE

SmallString<32> XCOFF::parseParmsType(uint32_t Value, unsigned ParmsNum) {
  SmallString<32> ParmsType;
  for (unsigned I = 0; I < ParmsNum; ++I) {
    if (I != 0)
      ParmsType += ", ";
    if ((Value & TracebackTable::ParmTypeIsFloatingBit) == 0) {
      // Fixed parameter type.
      ParmsType += "i";
      Value <<= 1;
    } else {
      if ((Value & TracebackTable::ParmTypeFloatingIsDoubleBit) == 0)
        // Float parameter type.
        ParmsType += "f";
      else
        // Double parameter type.
        ParmsType += "d";

      Value <<= 2;
    }
  }
  assert(Value == 0u && "ParmsType encodes more than ParmsNum parameters.");
  return ParmsType;
}

#undef RELOC_CASE
