#ifndef SYSTEMC64_H
#define SYSTEMC64_H

#include "systemmos6502.h"

class SystemC64 : public SystemMOS6502
{
public:
    SystemC64(CIniFile* settings, CIniFile* proj) : SystemMOS6502(settings, proj) {
        m_processor = MOS6502;
        m_system = C64;

        m_labels.append(SystemLabel(SystemLabel::ZEROPAGE,"Zero pages",0,0x00FF));
        m_labels.append(SystemLabel(SystemLabel::STACK,"Stack",0x0100,0x01FF));
        m_labels.append(SystemLabel(SystemLabel::BASIC,"Basic",0x0200,0x03FF));
        m_labels.append(SystemLabel(SystemLabel::SCREEN,"Screen bank 0",0x0400,0x07FF));
        m_labels.append(SystemLabel(SystemLabel::FREE,"Free",0x0800,0xBFFF));
        m_labels.append(SystemLabel(SystemLabel::SID,"SID",0xD400,0xD7FF));
        m_labels.append(SystemLabel(SystemLabel::COLOUR,"Colour ram",0xD800,0xDBFF));
        m_labels.append(SystemLabel(SystemLabel::FREE,"Free",0xE000,0xFFFE));
    }
};


class SystemX16 : public SystemMOS6502
{
public:
    SystemX16(CIniFile* settings, CIniFile* proj) : SystemMOS6502(settings, proj) {
        m_processor = MOS6502;
        m_system = X16;
    }
};

#endif // SYSTEMC64_H
