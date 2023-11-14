#pragma once

#include <map>

#include "IConvertableToHttp.h"

class ServerContent : public IConvertableToHttp
{
public:
	virtual std::string GetHttp() const override;

	template <class... Args>
	std::multimap<std::string, std::string>::iterator emplace(Args&&... args)
	{
		return m_data.emplace((std::forward<Args>(args))...);
	}

	std::multimap<std::string, std::string>::iterator begin();
	std::multimap<std::string, std::string>::iterator end();
	bool empty() const;

private:
	std::multimap<std::string, std::string> m_data; // representation of uploaded data
	const char* _body_begin = "<!DOCTYPE html>"\
							  "<html>" \
							  "<head>" \
								"<meta charset = \"UTF-8\">" \
								"<title>Server Content</title>" \
							  "</head>" \
							  "<body>\n";
	const char* _end = "</body>\n</html>";
};