// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

//*****************************************************************************
// NSUtilPriv.h
//
// Helpers for converting namespace separators.
//*****************************************************************************

#ifndef __NSUTILPRIV_H__
#define __NSUTILPRIV_H__

template <class T> class CQuickArray;
class SString;

struct ns
{
//*****************************************************************************
// Determine how many chars large a fully qualified name would be given the
// two parts of the name.  The return value includes room for every character
// in both names, as well as room for the separator and a final terminator.
//*****************************************************************************
static
int GetFullLength(                      // Number of chars in full name.
    const WCHAR *szNameSpace,           // Namspace for value.
    const WCHAR *szName);               // Name of value.

static
int GetFullLength(                      // Number of chars in full name.
    LPCUTF8     szNameSpace,            // Namspace for value.
    LPCUTF8     szName);                // Name of value.

//*****************************************************************************
// Scan the given string to see if the name contains any invalid characters
// that are not allowed.
//*****************************************************************************
static
int IsValidName(                        // true if valid, false invalid.
    const WCHAR *szName);               // Name to parse.

static
int IsValidName(                        // true if valid, false invalid.
    LPCUTF8     szName);                // Name to parse.

//*****************************************************************************
// Scan the string from the rear looking for the first valid separator.  If
// found, return a pointer to it.  Else return null.  This code is smart enough
// to skip over special sequences, such as:
//      a.b..ctor
//         ^
//         |
// The ".ctor" is considered one token.
//*****************************************************************************
static
WCHAR *FindSep(                         // Pointer to separator or null.
    const WCHAR *szPath);               // The path to look in.

static
LPUTF8 FindSep(                         // Pointer to separator or null.
    LPCUTF8     szPath);                // The path to look in.

//*****************************************************************************
// Take a path and find the last separator (nsFindSep), and then replace the
// separator with a '\0' and return a pointer to the name.  So for example:
//      a.b.c
// becomes two strings "a.b" and "c" and the return value points to "c".
//*****************************************************************************
static
LPUTF8       SplitInline(               // Pointer to name portion.
    __inout __inout_z LPUTF8      szPath);                // The path to split.

static
void SplitInline(
    __inout __inout_z LPUTF8       szPath,                // Path to split.
    LPCUTF8      &szNameSpace,          // Return pointer to namespace.
    LPCUTF8      &szName);              // Return pointer to name.

//*****************************************************************************
// Split the last parsable element from the end of the string as the name,
// the first part as the namespace.
//*****************************************************************************
static
int SplitPath(                          // true ok, false trunction.
    const WCHAR *szPath,                // Path to split.
    _Out_writes_opt_ (cchNameSpace) WCHAR *szNameSpace,           // Output for namespace value.
    int         cchNameSpace,           // Max chars for output.
    _Out_writes_opt_ (cchName) WCHAR       *szName,                // Output for name.
    int         cchName);               // Max chars for output.

static
int SplitPath(                          // true ok, false trunction.
    LPCUTF8     szPath,                 // Path to split.
    _Out_writes_opt_ (cchNameSpace) LPUTF8 szNameSpace,            // Output for namespace value.
    int         cchNameSpace,           // Max chars for output.
    _Out_writes_opt_ (cchName) LPUTF8      szName,                 // Output for name.
    int         cchName);               // Max chars for output.

//*****************************************************************************
// Take two values and put them together in a fully qualified path using the
// correct separator.
//*****************************************************************************
static
int MakePath(                           // true ok, false truncation.
    _Out_writes_(cchChars) WCHAR       *szOut,                 // output path for name.
    int         cchChars,               // max chars for output path.
    const WCHAR *szNameSpace,           // Namespace.
    const WCHAR *szName);               // Name.

static
int MakePath(                           // true ok, false truncation.
    _Out_writes_(cchChars) LPUTF8      szOut,                  // output path for name.
    int         cchChars,               // max chars for output path.
    LPCUTF8     szNameSpace,            // Namespace.
    LPCUTF8     szName);                // Name.

static
void MakePath(                          // throws on out of memory
    SString       &ssBuf,               // Where to put results.
    const SString &ssNameSpace,         // Namespace for name.
    const SString &ssName);             // Final part of name.

//*****************************************************************************
// Concatinate type names to assembly names
//*****************************************************************************
static
bool MakeAssemblyQualifiedName(                                  // true if ok, false if out of memory
                               CQuickBytes &qb,                  // location to put result
                               const WCHAR *szTypeName,          // Type name
                               const WCHAR *szAssemblyName);     // Assembly Name

static
bool MakeAssemblyQualifiedName(                                        // true ok, false truncation
                               _Out_writes_ (dwBuffer) WCHAR* pBuffer, // Buffer to receive the results
                               int    dwBuffer,                        // Number of characters total in buffer
                               const WCHAR *szTypeName,                // Namespace for name.
                               int   dwTypeName,                       // Number of characters (not including null)
                               const WCHAR *szAssemblyName,            // Final part of name.
                               int   dwAssemblyName);                  // Number of characters (not including null)
}; // struct ns

#define NAMESPACE_SEPARATOR_CHAR '.'
#define NAMESPACE_SEPARATOR_WCHAR W('.')
#define NAMESPACE_SEPARATOR_STR "."
#define NAMESPACE_SEPARATOR_WSTR W(".")

#define MAKE_FULL_PATH_ON_STACK_UTF8(toptr, pnamespace, pname) \
{ \
    int __i##toptr = ns::GetFullLength(pnamespace, pname); \
    toptr = (char *) alloca(__i##toptr); \
    ns::MakePath(toptr, __i##toptr, pnamespace, pname); \
}

#define MAKE_FULLY_QUALIFIED_MEMBER_NAME(ptr, pszNameSpace, pszClassName, pszMemberName, pszSig) \
{ \
    int __i##ptr = ns::GetFullLength(pszNameSpace, pszClassName); \
    __i##ptr += (pszMemberName ? (int) strlen(pszMemberName) : 0); \
    __i##ptr += (int)strlen(NAMESPACE_SEPARATOR_STR) * 2; \
    __i##ptr += (pszSig ? (int) strlen(pszSig) : 0); \
    ptr = (LPUTF8) alloca(__i##ptr); \
    ns::MakePath(ptr, __i##ptr, pszNameSpace, pszClassName); \
    if (pszMemberName) { \
        strcat_s(ptr, __i##ptr, NAMESPACE_SEPARATOR_STR); \
        strcat_s(ptr, __i##ptr, pszMemberName); \
    } \
    if (pszSig) { \
        if (! pszMemberName) \
            strcat_s(ptr, __i##ptr, NAMESPACE_SEPARATOR_STR); \
        strcat_s(ptr, __i##ptr, pszSig); \
    } \
}

#endif // __NSUTILPRIV_H__
