#ifndef TINYXMLHLP_H
#define TINYXMLHLP_H

#include <string>
#include <memory>

#include "tinyxml/tinyxml.h"

struct XmlDocument
{
private:
	XmlDocument(const XmlDocument&);
	XmlDocument& operator=(const XmlDocument&);

public:
	XmlDocument():mDocument(NULL),mRoot(NULL){ }

	~XmlDocument()
	{
		mRoot = NULL;
		delete mDocument;
		mDocument = NULL;
	}

	bool IsValid()const { return mRoot!=NULL; }
		
	TiXmlDocument*	mDocument; //!< AGGREGATION.
	TinyXmlElement* mRoot; //!< HANDLE.
};

class TinyXmlHlp
{
public:

	static std::string GetAttribute(TinyXmlElement* nod, std::string attributeName, std::string notFound = "")
	{
		if(nod && nod->Attribute(attributeName.c_str()))
		{
			return nod->Attribute(attributeName.c_str());
		}
		return notFound;
	}

	static double GetAttributeD(TinyXmlElement* nod, std::string pAttributeName, double pDefaut)
	{
		double ret = pDefaut;
		if(nod && nod->Attribute(pAttributeName.c_str()))
		{
			nod->Attribute(pAttributeName.c_str(), &ret);
		}
		return ret;
	}

	static int GetAttributeI(TinyXmlElement* nod, std::string pAttributeName, int pDefaut)
	{
		int ret = pDefaut;
		if(nod && nod->Attribute(pAttributeName.c_str()))
		{
			nod->Attribute(pAttributeName.c_str(), &ret);
		}
		return ret;
	}

	static std::auto_ptr<XmlDocument> OpenXmlFile(std::string pFileName, std::string pRootName, std::string &pOutOpeningErrors)
	{
		std::auto_ptr<XmlDocument> lDocument(new XmlDocument());
		lDocument->mDocument = new TiXmlDocument(pFileName.c_str());
		if(lDocument->mDocument->LoadFile(pFileName.c_str()))
		{
			lDocument->mRoot = lDocument->mDocument->FirstChildElement(pRootName.c_str());
			if(NULL == lDocument->mRoot)
			{
				pOutOpeningErrors = "Cannot find Xml Root in " + pFileName;
				lDocument.reset();
			}
		}
		else
		{
			pOutOpeningErrors = "Cannot load " + pFileName;
			lDocument.reset();
		}
		return lDocument;
	}
};

#endif // TINYXMLHLP_H