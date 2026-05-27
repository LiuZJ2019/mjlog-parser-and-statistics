/**
 * @file        mjlog_xml_parser.cpp
 * @brief       xml解析器的封装
 * @copyright   Copyright (c) 2026, Hasaki. All rights reserved.
 * @license     MIT License (see LICENSE file for details)
 * @author      间宫羽咲sama (Hasaki)
 * @date        2026-05-05
 * @version     1.0.0
 * @details     xml解析器的封装，基于tinyxml2库，把xmlStr解析成xml::XMLElement
 */
#include "mjlog_xml_parser.h"
#include "util/util.h"
#include "thirdparty/tinyxml2.h"


namespace Hasaki {

XmlParser::XmlParser(const string &xml_str)
{
    m_str = xml_str;
    xml::XMLError error = m_doc.Parse(xml_str.c_str(), xml_str.size());
    if (error != xml::XML_SUCCESS) {
        throw runtime_error("xml parse error: "s + xml::XMLDocument::ErrorIDToName(error));
    }

    auto root_xml = m_doc.RootElement();
    if (!root_xml) {
        throw runtime_error("xml parse error: no root node");
    }

    auto ver = root_xml->Attribute("ver");
    if (!ver || ver != "2.3"s) {
        throw runtime_error("Invalid xml version");
    }

    m_now = root_xml->FirstChildElement();
    if (!m_now) {
        throw runtime_error("xml parse error: no child node");
    }
}

pair<string, XmlMap> XmlParser::getOneNode() const
{
    if (m_now == nullptr) {
        return make_pair("", XmlMap{});
    }

    string name{m_now->Name()};
    XmlMap xmlMap{getXmlAttrs(m_now)};
    return {name, xmlMap};
}

void XmlParser::nextNode() {
    m_now = m_now->NextSiblingElement();
}

bool XmlParser::isValid() const noexcept
{
    return m_now != nullptr;
}

}   // namespace Hasaki
