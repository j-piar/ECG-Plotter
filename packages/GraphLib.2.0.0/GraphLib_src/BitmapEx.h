// BitmapEx.h: Schnittstelle f? die Klasse CBitmapEx.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BITMAPEX_H__10F9E5D7_F111_430A_BD87_4EC33416F455__INCLUDED_)
#define AFX_BITMAPEX_H__10F9E5D7_F111_430A_BD87_4EC33416F455__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*#ifdef _AFXDLL
class __declspec(dllexport)  CBitmapEx : public CBitmap
#else
class __declspec(dllimport)  CBitmapEx : public CBitmap
#endif */
class CBitmapEx : public CBitmap
{
	DECLARE_SERIAL( CBitmapEx )

public:

	CBitmapEx() {};
	virtual ~CBitmapEx() {};

	virtual void Serialize( CArchive& archive );

};

#endif // !defined(AFX_BITMAPEX_H__10F9E5D7_F111_430A_BD87_4EC33416F455__INCLUDED_)
