/**
 * @file        mjlog_xml_parser.h
 * @brief       xml解析器的封装
 * @copyright   Copyright (c) 2026, Hasaki. All rights reserved.
 * @license     MIT License (see LICENSE file for details)
 * @author      间宫羽咲sama (Hasaki)
 * @date        2026-05-05
 * @version     1.0.0
 * @details     xml解析器的封装，基于tinyxml2库
 */
#ifndef HASAKI_MJLOG_PARSER_PARSER_MJLOG_XML_PARSER_H_
#define HASAKI_MJLOG_PARSER_PARSER_MJLOG_XML_PARSER_H_

#include "util/util.h"
#include "thirdparty/tinyxml2.h"


namespace Hasaki {

struct XmlParser {
public:
    XmlParser(const string &xml_str);
    pair<string, XmlMap> getOneNode() const;
    void nextNode();
    bool isValid() const noexcept;

    string m_str;
    xml::XMLDocument m_doc;
    const xml::XMLElement *m_now;
};

}   // namespace Hasaki

#endif //HASAKI_MJLOG_PARSER_PARSER_MJLOG_XML_PARSER_H_
