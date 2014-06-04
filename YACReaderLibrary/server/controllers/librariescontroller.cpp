#include "librariescontroller.h"
#include "db_helper.h"  //get libraries
#include "yacreader_libraries.h"

#include "template.h"
#include "../static.h"


LibrariesController::LibrariesController() {}

void LibrariesController::service(HttpRequest& request, HttpResponse& response)
{
	HttpSession session=Static::sessionStore->getSession(request,response);
	session.set("ySession","ok");
	session.clearNavigationPath();
	response.setHeader("Content-Type", "text/html; charset=ISO-8859-1");
	response.setHeader("Connection","close");
	
	
	QString postData = QString::fromUtf8(request.getBody());
	//response.writeText(postData);

	QList<QString> data = postData.split("\n");
	if(data.length() > 2)
	{
		session.setDeviceType(data.at(0).split(":").at(1));
		session.setDisplayType(data.at(1).split(":").at(1));
		QList<QString> comics = data.at(2).split(":").at(1).split("\t");
		foreach(QString hash,comics)
		{
			session.setComicOnDevice(hash);
		}
	}
	else //valores por defecto, con propositos de depuración
	{
		session.setDeviceType("ipad");
		session.setDisplayType("retina");
	}

	Template t=Static::templateLoader->getTemplate("libraries_"+session.getDeviceType(),request.getHeader("Accept-Language"));
	t.enableWarnings();

	YACReaderLibraries libraries = DBHelper::getLibraries();
	QList<QString> names = DBHelper::getLibrariesNames();

	t.loop("library",names.length());

	int currentId = 0;
	int i = 0;
	foreach (QString name,names) {
		currentId = libraries.getId(name);
		t.setVariable(QString("library%1.name").arg(i),QString::number(currentId));
		t.setVariable(QString("library%1.label").arg(i),name);
		i++;
	}

	response.write(t.toLatin1(),true);
	
}
