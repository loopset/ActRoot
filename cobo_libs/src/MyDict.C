// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME dImediadIDatosdIApuntesUSCdITESEdIACTAR_trackingdIanalysis_2022dIACTAR_ANALYSIS_LIGHT_root6dIsrcdIMyDict
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Header files passed as explicit arguments
#include "/media/Datos/ApuntesUSC/TESE/ACTAR_tracking/analysis_2022/ACTAR_ANALYSIS_LIGHT_root6/inc/MEventReduced.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_ReducedData(void *p = nullptr);
   static void *newArray_ReducedData(Long_t size, void *p);
   static void delete_ReducedData(void *p);
   static void deleteArray_ReducedData(void *p);
   static void destruct_ReducedData(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::ReducedData*)
   {
      ::ReducedData *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::ReducedData >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("ReducedData", ::ReducedData::Class_Version(), "inc/MEventReduced.h", 12,
                  typeid(::ReducedData), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::ReducedData::Dictionary, isa_proxy, 4,
                  sizeof(::ReducedData) );
      instance.SetNew(&new_ReducedData);
      instance.SetNewArray(&newArray_ReducedData);
      instance.SetDelete(&delete_ReducedData);
      instance.SetDeleteArray(&deleteArray_ReducedData);
      instance.SetDestructor(&destruct_ReducedData);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::ReducedData*)
   {
      return GenerateInitInstanceLocal((::ReducedData*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::ReducedData*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_MEventReduced(void *p = nullptr);
   static void *newArray_MEventReduced(Long_t size, void *p);
   static void delete_MEventReduced(void *p);
   static void deleteArray_MEventReduced(void *p);
   static void destruct_MEventReduced(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::MEventReduced*)
   {
      ::MEventReduced *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::MEventReduced >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("MEventReduced", ::MEventReduced::Class_Version(), "inc/MEventReduced.h", 27,
                  typeid(::MEventReduced), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::MEventReduced::Dictionary, isa_proxy, 4,
                  sizeof(::MEventReduced) );
      instance.SetNew(&new_MEventReduced);
      instance.SetNewArray(&newArray_MEventReduced);
      instance.SetDelete(&delete_MEventReduced);
      instance.SetDeleteArray(&deleteArray_MEventReduced);
      instance.SetDestructor(&destruct_MEventReduced);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::MEventReduced*)
   {
      return GenerateInitInstanceLocal((::MEventReduced*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::MEventReduced*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr ReducedData::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *ReducedData::Class_Name()
{
   return "ReducedData";
}

//______________________________________________________________________________
const char *ReducedData::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::ReducedData*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int ReducedData::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::ReducedData*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *ReducedData::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::ReducedData*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *ReducedData::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::ReducedData*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr MEventReduced::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *MEventReduced::Class_Name()
{
   return "MEventReduced";
}

//______________________________________________________________________________
const char *MEventReduced::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::MEventReduced*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int MEventReduced::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::MEventReduced*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *MEventReduced::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::MEventReduced*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *MEventReduced::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::MEventReduced*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void ReducedData::Streamer(TBuffer &R__b)
{
   // Stream an object of class ReducedData.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(ReducedData::Class(),this);
   } else {
      R__b.WriteClassBuffer(ReducedData::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_ReducedData(void *p) {
      return  p ? new(p) ::ReducedData : new ::ReducedData;
   }
   static void *newArray_ReducedData(Long_t nElements, void *p) {
      return p ? new(p) ::ReducedData[nElements] : new ::ReducedData[nElements];
   }
   // Wrapper around operator delete
   static void delete_ReducedData(void *p) {
      delete ((::ReducedData*)p);
   }
   static void deleteArray_ReducedData(void *p) {
      delete [] ((::ReducedData*)p);
   }
   static void destruct_ReducedData(void *p) {
      typedef ::ReducedData current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::ReducedData

//______________________________________________________________________________
void MEventReduced::Streamer(TBuffer &R__b)
{
   // Stream an object of class MEventReduced.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(MEventReduced::Class(),this);
   } else {
      R__b.WriteClassBuffer(MEventReduced::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_MEventReduced(void *p) {
      return  p ? new(p) ::MEventReduced : new ::MEventReduced;
   }
   static void *newArray_MEventReduced(Long_t nElements, void *p) {
      return p ? new(p) ::MEventReduced[nElements] : new ::MEventReduced[nElements];
   }
   // Wrapper around operator delete
   static void delete_MEventReduced(void *p) {
      delete ((::MEventReduced*)p);
   }
   static void deleteArray_MEventReduced(void *p) {
      delete [] ((::MEventReduced*)p);
   }
   static void destruct_MEventReduced(void *p) {
      typedef ::MEventReduced current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::MEventReduced

namespace ROOT {
   static TClass *vectorlEfloatgR_Dictionary();
   static void vectorlEfloatgR_TClassManip(TClass*);
   static void *new_vectorlEfloatgR(void *p = nullptr);
   static void *newArray_vectorlEfloatgR(Long_t size, void *p);
   static void delete_vectorlEfloatgR(void *p);
   static void deleteArray_vectorlEfloatgR(void *p);
   static void destruct_vectorlEfloatgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<float>*)
   {
      vector<float> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<float>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<float>", -2, "vector", 423,
                  typeid(vector<float>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEfloatgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<float>) );
      instance.SetNew(&new_vectorlEfloatgR);
      instance.SetNewArray(&newArray_vectorlEfloatgR);
      instance.SetDelete(&delete_vectorlEfloatgR);
      instance.SetDeleteArray(&deleteArray_vectorlEfloatgR);
      instance.SetDestructor(&destruct_vectorlEfloatgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<float> >()));

      ::ROOT::AddClassAlternate("vector<float>","std::vector<float, std::allocator<float> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<float>*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEfloatgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<float>*)nullptr)->GetClass();
      vectorlEfloatgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEfloatgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEfloatgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<float> : new vector<float>;
   }
   static void *newArray_vectorlEfloatgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<float>[nElements] : new vector<float>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEfloatgR(void *p) {
      delete ((vector<float>*)p);
   }
   static void deleteArray_vectorlEfloatgR(void *p) {
      delete [] ((vector<float>*)p);
   }
   static void destruct_vectorlEfloatgR(void *p) {
      typedef vector<float> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<float>

namespace ROOT {
   static TClass *vectorlEReducedDatagR_Dictionary();
   static void vectorlEReducedDatagR_TClassManip(TClass*);
   static void *new_vectorlEReducedDatagR(void *p = nullptr);
   static void *newArray_vectorlEReducedDatagR(Long_t size, void *p);
   static void delete_vectorlEReducedDatagR(void *p);
   static void deleteArray_vectorlEReducedDatagR(void *p);
   static void destruct_vectorlEReducedDatagR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<ReducedData>*)
   {
      vector<ReducedData> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<ReducedData>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<ReducedData>", -2, "vector", 423,
                  typeid(vector<ReducedData>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEReducedDatagR_Dictionary, isa_proxy, 0,
                  sizeof(vector<ReducedData>) );
      instance.SetNew(&new_vectorlEReducedDatagR);
      instance.SetNewArray(&newArray_vectorlEReducedDatagR);
      instance.SetDelete(&delete_vectorlEReducedDatagR);
      instance.SetDeleteArray(&deleteArray_vectorlEReducedDatagR);
      instance.SetDestructor(&destruct_vectorlEReducedDatagR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<ReducedData> >()));

      ::ROOT::AddClassAlternate("vector<ReducedData>","std::vector<ReducedData, std::allocator<ReducedData> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<ReducedData>*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEReducedDatagR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<ReducedData>*)nullptr)->GetClass();
      vectorlEReducedDatagR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEReducedDatagR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEReducedDatagR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<ReducedData> : new vector<ReducedData>;
   }
   static void *newArray_vectorlEReducedDatagR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<ReducedData>[nElements] : new vector<ReducedData>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEReducedDatagR(void *p) {
      delete ((vector<ReducedData>*)p);
   }
   static void deleteArray_vectorlEReducedDatagR(void *p) {
      delete [] ((vector<ReducedData>*)p);
   }
   static void destruct_vectorlEReducedDatagR(void *p) {
      typedef vector<ReducedData> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<ReducedData>

namespace {
  void TriggerDictionaryInitialization_MyDict_Impl() {
    static const char* headers[] = {
"inc/MEventReduced.h",
nullptr
    };
    static const char* includePaths[] = {
"/usr/include",
"/media/Datos/ApuntesUSC/TESE/ACTAR_tracking/analysis_2022/ACTAR_ANALYSIS_LIGHT_root6/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "MyDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class __attribute__((annotate("$clingAutoload$inc/MEventReduced.h")))  ReducedData;
class __attribute__((annotate("$clingAutoload$inc/MEventReduced.h")))  MEventReduced;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "MyDict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "inc/MEventReduced.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"MEventReduced", payloadCode, "@",
"ReducedData", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("MyDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_MyDict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_MyDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_MyDict() {
  TriggerDictionaryInitialization_MyDict_Impl();
}
