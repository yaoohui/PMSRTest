

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Tue Jul 19 09:34:52 2016
 */
/* Compiler settings for PMSRTest.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0603 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __PMSRTest_h_h__
#define __PMSRTest_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IPMSRTest_FWD_DEFINED__
#define __IPMSRTest_FWD_DEFINED__
typedef interface IPMSRTest IPMSRTest;

#endif 	/* __IPMSRTest_FWD_DEFINED__ */


#ifndef __PMSRTest_FWD_DEFINED__
#define __PMSRTest_FWD_DEFINED__

#ifdef __cplusplus
typedef class PMSRTest PMSRTest;
#else
typedef struct PMSRTest PMSRTest;
#endif /* __cplusplus */

#endif 	/* __PMSRTest_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __PMSRTest_LIBRARY_DEFINED__
#define __PMSRTest_LIBRARY_DEFINED__

/* library PMSRTest */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_PMSRTest;

#ifndef __IPMSRTest_DISPINTERFACE_DEFINED__
#define __IPMSRTest_DISPINTERFACE_DEFINED__

/* dispinterface IPMSRTest */
/* [uuid] */ 


EXTERN_C const IID DIID_IPMSRTest;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("A41ADA07-9238-4D40-948F-6B62ACB076AD")
    IPMSRTest : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IPMSRTestVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPMSRTest * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPMSRTest * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPMSRTest * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IPMSRTest * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IPMSRTest * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IPMSRTest * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IPMSRTest * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } IPMSRTestVtbl;

    interface IPMSRTest
    {
        CONST_VTBL struct IPMSRTestVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPMSRTest_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPMSRTest_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPMSRTest_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPMSRTest_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IPMSRTest_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IPMSRTest_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IPMSRTest_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IPMSRTest_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_PMSRTest;

#ifdef __cplusplus

class DECLSPEC_UUID("08E80809-3712-48C0-BFE7-8FB611FB45EA")
PMSRTest;
#endif
#endif /* __PMSRTest_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


