/*
    Copyright (C) 2018 Sebastian J. Wolf

    This file is part of Zaster.

    Zaster is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Zaster is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Zaster. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MESSAGECONSTANTS_H
#define MESSAGECONSTANTS_H

// Placeholder message length, that will be changed when we completed the whole message
const char MESSAGE_LENGTH_PLACEHOLDER[] = "000000000000";
const char SIGNATURE_CONTROL_REFERENCE[] = "4242";
const char SYSTEM_IDENTIFICATION[] = "0";
const char SEGMENT_MESSAGE_HEADER_ID[] = "HNHBK";
const char SEGMENT_MESSAGE_HEADER_VERSION[] = "3";
const char SEGMENT_IDENTIFICATION_ID[] = "HKIDN";
const char SEGMENT_IDENTIFICATION_VERSION[] = "2";
const char SEGMENT_PROCESS_PREPARATION_ID[] = "HKVVB";
const char SEGMENT_PROCESS_PREPARATION_VERSION[] = "3";
const char SEGMENT_MESSAGE_TERMINATION_ID[] = "HNHBS";
const char SEGMENT_MESSAGE_TERMINATION_VERSION[] = "1";
const char SEGMENT_MESSAGE_FEEDBACK_ID[] = "HIRMG";
const char SEGMENT_MESSAGE_FEEDBACK_VERSION[] = "2";
const char SEGMENT_SEGMENT_FEEDBACK_ID[] = "HIRMS";
const char SEGMENT_SEGMENT_FEEDBACK_VERSION[] = "2";
const char SEGMENT_BANK_PARAMETER_ID[] = "HIBPA";
const char SEGMENT_BANK_PARAMETER_VERSION[] = "3";
const char SEGMENT_SECURITY_PROCEDURE_ID[] = "HISHV";
const char SEGMENT_SECURITY_PROCEDURE_VERSION[] = "3";
const char SEGMENT_DIALOG_END_ID[] = "HKEND";
const char SEGMENT_DIALOG_END_VERSION[] = "1";
const char SEGMENT_SIGNATURE_HEADER_ID[] = "HNSHK";
const char SEGMENT_SIGNATURE_HEADER_VERSION[] = "4";
const char SEGMENT_SIGNATURE_FOOTER_ID[] = "HNSHA";
const char SEGMENT_SIGNATURE_FOOTER_VERSION[] = "2";
const char SEGMENT_ENCRYPTION_HEADER_ID[] = "HNVSK";
const char SEGMENT_ENCRYPTION_HEADER_VERSION[] = "3";
const char SEGMENT_ENCRYPTED_DATA_ID[] = "HNVSD";
const char SEGMENT_ENCRYPTED_DATA_VERSION[] = "1";
const char SEGMENT_USER_PARAMETER_DATA_ID[] = "HIUPA";
const char SEGMENT_USER_PARAMETER_DATA_VERSION[] = "4";
const char SEGMENT_ACCOUNT_INFORMATION_ID[] = "HIUPD";
const char SEGMENT_ACCOUNT_INFORMATION_VERSION[] = "6";
const char SEGMENT_ACCOUNT_BALANCE_ID[] = "HKSAL";
const char SEGMENT_ACCOUNT_BALANCE_VERSION[] = "7";
const char SEGMENT_ACCOUNT_BALANCE_RESPONSE_ID[] = "HISAL";
const char SEGMENT_ACCOUNT_BALANCE_RESPONSE_VERSION[] = "7";
#endif // MESSAGECONSTANTS_H
