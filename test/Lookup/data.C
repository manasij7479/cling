//------------------------------------------------------------------------------
// CLING - the C++ LLVM-based InterpreterG :)
//
// This file is dual-licensed: you can choose to license it under the University
// of Illinois Open Source License or the GNU Lesser General Public License. See
// LICENSE.TXT for details.
//------------------------------------------------------------------------------

// RUN: cat %s | %cling 2>&1 | FileCheck %s
// Test lookupData

.rawInput 1
#include "cling/Interpreter/Interpreter.h"
#include "cling/Interpreter/LookupHelper.h"

#include "clang/AST/ASTContext.h"
#include "clang/AST/Expr.h"
#include "clang/Sema/Sema.h"

#include "llvm/ADT/SmallVector.h"

#include <iostream>
#include <cstdio>
#include <string>

void printScope(const clang::DeclContext *ctxt) {
//  llvm::raw_string_ostream OS(S);
//  dyn_cast<clang::NamedDecl>(D)
//  ->getNameForDiagnostic(OS, D->getASTContext().getPrintingPolicy(),
//                         /*Qualified=*/true);
  const clang::NamedDecl *decl = llvm::dyn_cast<clang::NamedDecl>(ctxt);
  if (decl) {
    std::cout << "Context is " << decl->getQualifiedNameAsString() << '\n';
  } else {
    std::cout << "Context is not a named decl\n";
  }
}

int gValue;

template<typename T> class aTemplate {
public:
   static T gValue;
   T fMember;
};

namespace NS {
   double gValue;
}

namespace NS {
  double gValue2;
}


class A {
   static short gValue;
   double fMember;
};
.rawInput 0


const cling::LookupHelper& lookup = gCling->getLookupHelper();
cling::LookupHelper::DiagSetting diags = cling::LookupHelper::WithDiagnostics;
std::string buf;
clang::PrintingPolicy Policy(gCling->getSema().getASTContext().getPrintingPolicy());

//
//  We need to fetch the global scope declaration,
//  otherwise known as the translation unit decl.
//
const clang::Decl* G = lookup.findScope("", diags);
printf("G: 0x%lx\n", (unsigned long) G);
//CHECK: G: 0x{{[1-9a-f][0-9a-f]*$}}

//
//  We need these class declarations.
//

const clang::Decl* class_tempFlt = lookup.findScope("aTemplate<float>", diags);
printf("class_tempFlt: 0x%lx\n", (unsigned long) class_tempFlt);
//CHECK: class_tempFlt: 0x{{[1-9a-f][0-9a-f]*$}}

const clang::Decl* class_A = lookup.findScope("A", diags);
printf("class_A: 0x%lx\n", (unsigned long) class_A);
//CHECK-NEXT: class_A: 0x{{[1-9a-f][0-9a-f]*$}}

const clang::Decl* namespace_NS = lookup.findScope("NS", diags);
printf("namespace_NS: 0x%lx\n", (unsigned long) namespace_NS);
//CHECK-NEXT: namespace_NS: 0x{{[1-9a-f][0-9a-f]*$}}

//
// Now test find data members.
//
const clang::ValueDecl *decl;

decl = lookup.findDataMember(G,"gValue", diags);
printScope(decl->getDeclContext());
decl->dump();
//CHECK-NEXT: Context is not a named decl
//CHECK-NEXT: VarDecl 0x{{[1-9a-f][0-9a-f]*}} <{{.*}}> gValue 'int'

decl = lookup.findDataMember(class_tempFlt,"gValue", diags);
printScope(decl->getDeclContext());
decl->dump();
//CHECK-NEXT: Context is aTemplate
//CHECK-NEXT: VarDecl 0x{{[1-9a-f][0-9a-f]*}} <{{.*}}> gValue 'float':'float' static

decl = lookup.findDataMember(class_tempFlt,"fMember", diags);
printScope(decl->getDeclContext());
decl->dump();
//CHECK-NEXT: Context is aTemplate
//CHECK-NEXT: FieldDecl 0x{{[1-9a-f][0-9a-f]*}} <{{.*}}> fMember 'float':'float'

decl = lookup.findDataMember(namespace_NS,"gValue", diags);
printScope(decl->getDeclContext());
decl->dump();
//CHECK-NEXT: Context is NS
//CHECK-NEXT: VarDecl 0x{{[1-9a-f][0-9a-f]*}} <{{.*}}> gValue 'double'

decl = lookup.findDataMember(namespace_NS,"gValue2", diags);
printScope(decl->getDeclContext());
decl->dump();
//CHECK-NEXT: Context is NS
//CHECK-NEXT: VarDecl 0x{{[1-9a-f][0-9a-f]*}} <{{.*}}> gValue2 'double'

decl = lookup.findDataMember(class_A,"gValue", diags);
printScope(decl->getDeclContext());
decl->dump();
//CHECK-NEXT: Context is A
//CHECK-NEXT: VarDecl 0x{{[1-9a-f][0-9a-f]*}} <{{.*}}> gValue 'short' static

decl = lookup.findDataMember(class_A,"fMember", diags);
printScope(decl->getDeclContext());
decl->dump();
//CHECK-NEXT: Context is A
//CHECK-NEXT: FieldDecl 0x{{[1-9a-f][0-9a-f]*}} <{{.*}}> fMember 'double'

.q
