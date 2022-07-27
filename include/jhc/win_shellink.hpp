/*******************************************************************************
*    C++ Common Library
*    ---------------------------------------------------------------------------
*    Copyright (C) 2022 JiangXueqiao <winsoft666@outlook.com>.
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef JHC_WIN_SHELLINK_HPP__
#define JHC_WIN_SHELLINK_HPP__
#pragma once

#include "jhc/config.hpp"
#include "jhc/arch.hpp"

#ifdef JHC_WIN
#include <vector>
#include <string>
#include "jhc/enum_flags.hpp"

// This class provides an interface for the MS Shell Link Binary File Format.
// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-shllink/16cb4ca1-9339-4d0c-a68d-bf1d6cc0f943
//
namespace jhc {
class WinShellink {
   public:
    enum class ShellinkErr {
        SHLLINK_ERR_NONE = 0x0,
        SHLLINK_ERR_FCL = 0x01,
        SHLLINK_ERR_FIO = 0x02,
        SHLLINK_ERR_NULLPA = 0x03,
        SHLLINK_ERR_WHEADS = 0x04,
        SHLLINK_ERR_WCLSIDS = 0x05,
        SHLLINK_ERR_NULLPIDL = 0x06,
        SHLLINK_ERR_NULLPIDLM = 0x07,
        SHLLINK_ERR_INVIDL = 0x08,
        SHLLINK_ERR_INVLIHS = 0x09,
        SHLLINK_ERR_VIDSLOW = 0x0A,
        SHLLINK_ERR_NULLPVIDD = 0x0B,
        SHLLINK_ERR_NULLPLBP = 0x0C,
        SHLLINK_ERR_NULLPCPS = 0x0D,
        SHLLINK_ERR_NULLPLBPU = 0x0E,
        SHLLINK_ERR_NULLPCPSU = 0x0F,
        SHLLINK_ERR_NULLPNETN = 0x10,
        SHLLINK_ERR_NULLPDEVN = 0x11,
        SHLLINK_ERR_NULLPNNU = 0x12,
        SHLLINK_ERR_NULLPDNU = 0x13,
        SHLLINK_ERR_NULLPSTRDNAME = 0x14,
        SHLLINK_ERR_NULLPSTRDRPATH = 0x15,
        SHLLINK_ERR_NULLPSTRDWDIR = 0x16,
        SHLLINK_ERR_NULLPSTRDARG = 0x17,
        SHLLINK_ERR_NULLPSTRDICO = 0x18,
        SHLLINK_ERR_UNKEDBSIG = 0x19,
        SHLLINK_DUPEEX_ConsoleDataBlock = 0x1A,
        SHLLINK_DUPEEX_ConsoleFEDataBlock = 0x1B,
        SHLLINK_DUPEEX_DarwinDataBlock = 0x1C,
        SHLLINK_DUPEEX_EnvironmentVariableDataBlock = 0x1D,
        SHLLINK_DUPEEX_IconEnvironmentDataBlock = 0x1E,
        SHLLINK_DUPEEX_KnownFolderDataBlock = 0x1F,
        SHLLINK_DUPEEX_PropertyStoreDataBlock = 0x20,
        SHLLINK_DUPEEX_ShimDataBlock = 0x21,
        SHLLINK_DUPEEX_SpecialFolderDataBlock = 0x22,
        SHLLINK_DUPEEX_TrackerDataBlock = 0x23,
        SHLLINK_DUPEEX_VistaAndAboveIDListDataBlock = 0x24,
        SHLLINK_ERRX_WRONGSIZE = 0x25,
        SHLLINK_ERRX_WRONGVERSION = 0x26,
        SHLLINK_ERR_NULLPEXTD = 0x27,
        SHLLINK_ERRX_NULLPSTRFNAME = 0x28,
        SHLLINK_ERRX_NULLPSTRDARDA = 0x29,
        SHLLINK_ERRX_NULLPSTRDARDU = 0x2A,
        SHLLINK_ERRX_NULLPSTRENVDA = 0x2B,
        SHLLINK_ERRX_NULLPSTRENVDU = 0x2C,
        SHLLINK_ERRX_NULLPSTRIENVDA = 0x2D,
        SHLLINK_ERRX_NULLPSTRIENVDU = 0x2E,
    };

#pragma region LinkHeader
    enum class ShllinkShowCmd : uint32_t {
        SC_SHOWNORMAL = 0x1,
        SC_SHOWMAXIMIZED = 0x00000003,
        SC_SHOWMINNOACTIVE = 0x00000007
    };

    enum class ShllinkLinkFlag : uint32_t {
        /*
        The shell link is saved with an item ID list (IDList). If this bit is set, a
        LinkTargetIDList structure (section 2.2) MUST follow the ShellLinkHeader.
        If this bit is not set, this structure MUST NOT be present
        */
        LF_HasLinkTargetIDList = 1 << 0,

        /*
        The shell link is saved with link information. If this bit is set, a LinkInfo
        structure (section 2.3) MUST be present. If this bit is not set, this structure
        MUST NOT be present
        */
        LF_HasLinkInfo = 1 << 1,

        /*
        The shell link is saved with a name string. If this bit is set, a
        NAME_STRING StringData structure (section 2.4) MUST be present. If
        this bit is not set, this structure MUST NOT be present
        */
        LF_HasName = 1 << 2,

        /*
        The shell link is saved with a relative path string. If this bit is set, a
        RELATIVE_PATH StringData structure (section 2.4) MUST be present. If
        this bit is not set, this structure MUST NOT be present
        */
        LF_HasRelativePath = 1 << 3,

        /*
        The shell link is saved with a working directory string. If this bit is set, a
        WORKING_DIR StringData structure (section 2.4) MUST be present. If
        this bit is not set, this structure MUST NOT be present
        */
        LF_HasWorkingDir = 1 << 4,

        /*
        The shell link is saved with command line arguments. If this bit is set, a
        COMMAND_LINE_ARGUMENTS StringData structure (section 2.4) MUST
        be present. If this bit is not set, this structure MUST NOT be present
        */
        LF_HasArguments = 1 << 5,

        /*
        The shell link is saved with an icon location string. If this bit is set, an
        ICON_LOCATION StringData structure (section 2.4) MUST be present. If
        this bit is not set, this structure MUST NOT be present
        */
        LF_HasIconLocation = 1 << 6,

        /*
        The shell link contains Unicode encoded strings. This bit SHOULD be set. If
        this bit is set, the StringData section contains Unicode-encoded strings;
        otherwise, it contains strings that are encoded using the system default
        code page
        */
        LF_IsUnicode = 1 << 7,

        /*
        The LinkInfo structure (section 2.3) is ignored
        */
        LF_ForceNoLinkInfo = 1 << 8,

        /*
        The shell link is saved with an
        EnvironmentVariableDataBlock (section 2.5.4)
        */
        LF_HasExpString = 1 << 9,

        /*
        The target is run in a separate virtual machine when launching a link
        target that is a 16-bit application
    */
        LF_RunInSeparateProcess = 1 << 10,

        // unused 1<<11
        /*
        The shell link is saved with a DarwinDataBlock (section 2.5.3)
        */
        LF_HasDarwinID = 1 << 12,

        /*
        The application is run as a different user when the target of the shell link is
        activated
        */
        LF_RunAsUser = 1 << 13,
        /*
        The shell link is saved with an IconEnvironmentDataBlock (section 2.5.5)
        */
        LF_HasExpIcon = 1 << 14,
        /*
        The file system location is represented in the shell namespace when the
        path to an item is parsed into an IDList
        */
        LF_NoPidlAlias = 1 << 15,
        // unused 1<<16
        /*
        The shell link is saved with a ShimDataBlock (section 2.5.8)
        */
        LF_RunWithShimLayer = 1 << 17,
        /*
        The TrackerDataBlock (section 2.5.10) is ignored
        */
        LF_ForceNoLinkTrack = 1 << 18,
        /*
        The shell link attempts to collect target properties and store them in the
        PropertyStoreDataBlock (section 2.5.7) when the link target is set
        */
        LF_EnableTargetMetadata = 1 << 19,
        /*
        The EnvironmentVariableDataBlock is ignored
        */
        LF_DisableLinkPathTracking = 1 << 20,
        /*
        The SpecialFolderDataBlock (section 2.5.9) and the KnownFolderDataBlock (section 2.5.6) are ignored when loading the shell
        link. If this bit is set, these extra data blocks SHOULD NOT be saved when
        saving the shell link
        */
        LF_DisableKnownFolderTracking = 1 << 21,
        /*
        If the link has a KnownFolderDataBlock (section 2.5.6), the unaliased form
        of the known folder IDList SHOULD be used when translating the target
        IDList at the time that the link is loaded
        */
        LF_DisableKnownFolderAlias = 1 << 22,
        /*
        Creating a link that references another link is enabled. Otherwise,
        specifying a link as the target IDList SHOULD NOT be allowed
    */
        LF_AllowLinkToLink = 1 << 23,
        /*
        When saving a link for which the target IDList is under a known folder,
        either the unaliased form of that known folder or the target IDList SHOULD
        be used
    */
        LF_UnaliasOnSave = 1 << 24,
        /*
        The target IDList SHOULD NOT be stored; instead, the path specified in the
        EnvironmentVariableDataBlock (section 2.5.4) SHOULD be used to refer to
        the target
    */
        LF_PreferEnvironmentPath = 1 << 25,
        /*
        When the target is a UNC name that refers to a location on a local
        machine, the local path IDList in the
        PropertyStoreDataBlock (section 2.5.7) SHOULD be stored, so it can be
        used when the link is loaded on the local machine
    */
        LF_KeepLocalIDListForUNCTarget = 1 << 26
        // 5 bits 0
    };
    ALLOW_FLAGS_FOR_ENUM_IN_CLASS(ShllinkLinkFlag);

    enum class FileAttribute : uint32_t {
        /*
        The file or directory is read-only. For a file, if this bit is set,
        applications can read the file but cannot write to it or delete
        it. For a directory, if this bit is set, applications cannot delete
        the directory
    */
        FA_READONLY = 1 << 0,
        /*
        The file or directory is read-only. For a file, if this bit is set,
        applications can read the file but cannot write to it or delete
        it. For a directory, if this bit is set, applications cannot delete
        the directory
    */
        FA_HIDDEN = 1 << 1,
        /*
        The file or directory is part of the operating system or is used
        exclusively by the operating system
    */
        FA_SYSTEM = 1 << 2,
        // reserved 1<<3 MUST be zero
        /*
        The link target is a directory instead of a file
    */
        FA_DIRECTORY = 1 << 4,
        /*
        The file or directory is an archive file. Applications use this
        flag to mark files for backup or removal
    */
        FA_ARCHIVE = 1 << 5,
        // reserved 1<<6 MUST be zero
        /*
        The file or directory has no other flags set. If this bit is 1, all
        other bits in this structure MUST be clear
    */
        FA_NORMAL = 1 << 7,
        /*
        The file is being used for temporary storage
    */
        FA_TEMPORARY = 1 << 8,
        /*
        The file is a sparse file
    */
        FA_SPARSE_FILE = 1 << 9,
        /*
        The file or directory has an associated reparse point
    */
        FA_REPARSE_POINT = 1 << 10,
        /*
        The file or directory is compressed. For a file, this means that
        all data in the file is compressed. For a directory, this means
        that compression is the default for newly created files and
        subdirectories
    */
        FA_COMPRESSED = 1 << 11,
        /*
        The data of the file is not immediately available
    */
        FA_OFFLINE = 1 << 12,
        /*
        The contents of the file need to be indexed
    */
        FA_NOT_CONTENT_INDEXED = 1 << 13,
        /*
        The file or directory is encrypted. For a file, this means that
        all data in the file is encrypted. For a directory, this means
        that encryption is the default for newly created files and
        subdirectories
    */
        FA_ENCRYPTED = 1 << 14,
        // 16 bit 0
    };
    ALLOW_FLAGS_FOR_ENUM_IN_CLASS(FileAttribute);

    enum ShllinkHotKey : uint8_t {
        HOTK_NONE = 0x00,
        HOTKF_SHIFT = 0x01,
        HOTKF_CONTROL = 0x02,
        HOTKF_ALT = 0x04,
        HOTK_0 = 0x30,
        HOTK_1 = 0x31,
        HOTK_2 = 0x32,
        HOTK_3 = 0x33,
        HOTK_4 = 0x34,
        HOTK_5 = 0x35,
        HOTK_6 = 0x36,
        HOTK_7 = 0x37,
        HOTK_8 = 0x38,
        HOTK_9 = 0x39,
        HOTK_A = 0x41,
        HOTK_B = 0x42,
        HOTK_C = 0x43,
        HOTK_D = 0x44,
        HOTK_E = 0x45,
        HOTK_F = 0x46,
        HOTK_G = 0x47,
        HOTK_H = 0x48,
        HOTK_I = 0x49,
        HOTK_J = 0x4A,
        HOTK_K = 0x4B,
        HOTK_L = 0x4C,
        HOTK_M = 0x4D,
        HOTK_N = 0x4E,
        HOTK_O = 0x4F,
        HOTK_P = 0x50,
        HOTK_Q = 0x51,
        HOTK_R = 0x52,
        HOTK_S = 0x53,
        HOTK_T = 0x54,
        HOTK_U = 0x55,
        HOTK_V = 0x56,
        HOTK_W = 0x57,
        HOTK_X = 0x58,
        HOTK_Y = 0x59,
        HOTK_Z = 0x5A,
        HOTK_F1 = 0x70,
        HOTK_F2 = 0x71,
        HOTK_F3 = 0x72,
        HOTK_F4 = 0x73,
        HOTK_F5 = 0x74,
        HOTK_F6 = 0x75,
        HOTK_F7 = 0x76,
        HOTK_F8 = 0x77,
        HOTK_F9 = 0x78,
        HOTK_F10 = 0x79,
        HOTK_F11 = 0x7A,
        HOTK_F12 = 0x7B,
        HOTK_F13 = 0x7C,
        HOTK_F14 = 0x7D,
        HOTK_F15 = 0x7E,
        HOTK_F16 = 0x7F,
        HOTK_F17 = 0x80,
        HOTK_F18 = 0x81,
        HOTK_F19 = 0x82,
        HOTK_F20 = 0x83,
        HOTK_F21 = 0x84,
        HOTK_F22 = 0x85,
        HOTK_F23 = 0x86,
        HOTK_F24 = 0x87,
        HOTK_NUMLOCK = 0x90,
        HOTK_SCROLL = 0x91
    };

    struct LinkHeader {
        // The size, in bytes, of this structure. This value MUST be 0x0000004C
        const uint32_t HeaderSize = 0x0000004C;

        // A class identifier (CLSID). This value MUST be 00021401-0000-0000-C000-000000000046
        uint64_t LinkCLSID_L = 0;
        uint64_t LinkCLSID_H = 0;

        // A LinkFlags structure (section 2.1.1) that specifies information about the shell link and the presence of optional portions of the structure
        ShllinkLinkFlags LinkFlags;

        // A FileAttributesFlags structure (section 2.1.2) that specifies information about the link target
        FileAttributes TargetFileAttributes;

        // A FILETIME structure ([MS-DTYP] section 2.3.3) that specifies the creation time of the link target in UTC (Coordinated Universal Time). If the value is zero, there is no creation time set on the link target
        uint64_t CreationTime = 0;

        // A FILETIME structure ([MS-DTYP] section 2.3.3) that specifies the access time of the link target in UTC (Coordinated Universal Time). If the value is zero, there is no access time set on the link target
        uint64_t AccessTime = 0;

        // A FILETIME structure ([MS-DTYP] section 2.3.3) that specifies the write time of the link target in UTC (Coordinated Universal Time). If the value is zero, there is no write time set on the link target
        uint64_t WriteTime = 0;

        // A 32-bit unsigned integer that specifies the size, in bytes, of the link target. If the link target file is larger than 0xFFFFFFFF, this value specifies the least significant 32 bits of the link target file size
        uint32_t FileSize = 0;

        // A 32-bit signed integer that specifies the index of an icon within a given icon location
        int32_t IconIndex = 0;

        /* A 32-bit unsigned integer that specifies the expected window state of an application launched by the link. This value SHOULD be one of the following

            SW_SHOWNORMAL       0x00000001   The application is open and its window is open in a normal fashion.
            SW_SHOWMAXIMIZED    0x00000003   The application is open, and keyboard focus is given to the application, but its window is not shown.
            SW_SHOWMINNOACTIVE  0x00000007   The application is open, but its window is not shown. It is not given the keyboard focus

            All other values MUST be treated as SW_SHOWNORMAL
        */
        uint32_t ShowCommand = 0;

        // A HotKeyFlags structure (section 2.1.3) that specifies the keystrokes used to launch the application referenced by the shortcut key. This value is assigned to the application after it is launched, so that pressing the key activates that application
        uint16_t HotKey = 0;

        /*
            + 10 bytes NULL
        */
    };
#pragma endregion LinkHeader

#pragma region LinkTargetIDList
    /*
        SHLLINK LinkTargetIDList
        - specifies the target of the link. The presence of this structure is specified by the HasLinkTargetIDList bit (LinkFlags section 2.1.1) in the ShellLinkHeader
    */
    struct LinkTargetIDList {
        // A stored IDList structure (section 2.2.1), which contains the item ID list
        std::vector<uint8_t> WholeIDList;
    };
#pragma endregion LinkTargetIDList

#pragma region LinkInfo
    enum class LinkInfoFlag : uint32_t {
        /*
        If set, the VolumeID and LocalBasePath fields are present,
        and their locations are specified by the values of the
        VolumeIDOffset and LocalBasePathOffset fields,
        respectively. If the value of the LinkInfoHeaderSize field is
        greater than or equal to 0x00000024, the
        LocalBasePathUnicode field is present, and its location is
        specified by the value of the LocalBasePathOffsetUnicode
        field.
        If not set, the VolumeID, LocalBasePath, and
        LocalBasePathUnicode fields are not present, and the
        values of the VolumeIDOffset and LocalBasePathOffset
        fields are zero. If the value of the LinkInfoHeaderSize field
        is greater than or equal to 0x00000024, the value of the
        LocalBasePathOffsetUnicode field is zero
    */
        LIF_VolumeIDAndLocalBasePath = 1 << 0,
        /*
        If set, the CommonNetworkRelativeLink field is present,
        and its location is specified by the value of the
        CommonNetworkRelativeLinkOffset field.
        If not set, the CommonNetworkRelativeLink field is not
        present, and the value of the
        CommonNetworkRelativeLinkOffset field is zero
    */
        LIF_CommonNetworkRelativeLinkAndPathSuffix = 1 << 1
        // 30 bit 0
    };
    ALLOW_FLAGS_FOR_ENUM_IN_CLASS(LinkInfoFlag);

    struct VolumeID {
        // A 32-bit, unsigned integer that specifies the size, in bytes, of this structure. This value MUST be greater than 0x00000010. All offsets specified in this structure MUST be less than this value, and all strings contained in this structure MUST fit within the extent defined by this size
        uint32_t VolumeIDSize = 0;
        // A 32-bit, unsigned integer that specifies the type of drive the link target is stored on
        /*
                DRIVE_UNKNOWN       0x00000000       The drive type cannot be determined.
                DRIVE_NO_ROOT_DIR   0x00000001       The root path is invalid; for example, there is no volume mounted at the path.
                DRIVE_REMOVABLE     0x00000002       The drive has removable media, such as a floppy drive, thumb drive, or flash card reader.
                DRIVE_FIXED         0x00000003       The drive has fixed media, such as a hard drive or flash drive.
                DRIVE_REMOTE        0x00000004       The drive is a remote (network) drive.
                DRIVE_CDROM         0x00000005       The drive is a CD-ROM drive.
                DRIVE_RAMDISK       0x00000006       The drive is a RAM disk.
            */
        uint32_t DriveType = 0;
        // A 32-bit, unsigned integer that specifies the drive serial number of the volume the link target is stored on
        uint32_t DriveSerialNumber = 0;
        // A 32-bit, unsigned integer that specifies the location of a string that contains the volume label of the drive that the link target is stored on. This value is an offset, in bytes, from the start of the VolumeID structure to a NULL-terminated string of characters, defined by the system default code page. The volume label string is located in the Data field of this structure. If the value of this field is 0x00000014, it MUST be ignored, and the value of the VolumeLabelOffsetUnicode field MUST be used to locate the volume label string
        uint32_t VolumeLabelOffset = 0;
        // An optional, 32-bit, unsigned integer that specifies the location of a string that contains the volume label of the drive that the link target is stored on. This value is an offset, in bytes, from the start of the VolumeID structure to a NULL-terminated string of Unicode characters. The volume label string is located in the Data field of this structure. If the value of the VolumeLabelOffset field is not 0x00000014, this field MUST NOT be present; instead, the value of the VolumeLabelOffset field MUST be used to locate the volume label string
        uint32_t VolumeLabelOffsetUnicode = 0;
        // A buffer of data that contains the volume label of the drive as a string defined by the system default code page or Unicode characters, as specified by preceding fields
        std::vector<char> Data;
    };

    enum class CommonNetworkRelativeLinkFlag : uint32_t {
        CNETRLNK_ValidDevice = 1 << 0,
        CNETRLNK_ValidNetType = 1 << 1
    };
    ALLOW_FLAGS_FOR_ENUM_IN_CLASS(CommonNetworkRelativeLinkFlag);

    struct CommonNetworkRelativeLink {
        // A 32-bit, unsigned integer that specifies the size, in bytes, of the CommonNetworkRelativeLink structure. This value MUST be greater than or equal to 0x00000014. All offsets specified in this structure MUST be less than this value, and all strings contained in this structure MUST fit within the extent defined by this size
        uint32_t CommonNetworkRelativeSize = 0;
        // Flags that specify the contents of the DeviceNameOffset and NetProviderType fields
        CommonNetworkRelativeLinkFlags ComNetRelLnkFlags;
        // A 32-bit, unsigned integer that specifies the location of the NetName field. This value is an offset, in bytes, from the start of the CommonNetworkRelativeLink structure
        uint32_t NetNameOffset = 0;
        // A 32-bit, unsigned integer that specifies the location of the DeviceName field. If the ValidDevice flag is set, this value is an offset, in bytes, from the start of the CommonNetworkRelativeLink structure; otherwise, this value MUST be zero
        uint32_t DeviceNameOffset = 0;
        // A 32-bit, unsigned integer that specifies the type of network provider. If the ValidNetType flag is set, this value MUST be one of the defined; otherwise, this value MUST be ignored
        uint32_t NetworkProviderType = 0;
        // An optional, 32-bit, unsigned integer that specifies the location of the NetNameUnicode field. This value is an offset, in bytes, from the start of the CommonNetworkRelativeLink structure. This field MUST be present if the value of the NetNameOffset field is greater than 0x00000014; otherwise, this field MUST NOT be present
        uint32_t NetNameOffsetUnicode = 0;
        // An optional, 32-bit, unsigned integer that specifies the location of the DeviceNameUnicode field. This value is an offset, in bytes, from the start of the CommonNetworkRelativeLink structure. This field MUST be present if the value of the NetNameOffset field is greater than 0x00000014; otherwise, this field MUST NOT be present
        uint32_t DeviceNameOffsetUnicode = 0;
        // A NULL每terminated string, as defined by the system default code page, which specifies a server share path; for example, "\\server\share"
        std::string NetName;
        // A NULL每terminated string, as defined by the system default code page, which specifies a device; for example, the drive letter "D:"
        std::string DeviceName;
        // An optional, NULL每terminated, Unicode string that is the Unicode version of the NetName string. This field MUST be present if the value of the NetNameOffset field is greater than 0x00000014; otherwise, this field MUST NOT be present
        std::wstring NetNameUnicode;
        // An optional, NULL每terminated, Unicode string that is the Unicode version of the DeviceName string. This field MUST be present if the value of the NetNameOffset field is greater than 0x00000014; otherwise, this field MUST NOT be present
        std::wstring DeviceNameUnicode;
    };

    struct LinkInfo {
        // A 32-bit, unsigned integer that specifies the size, in bytes, of the LinkInfo structure. All offsets specified in this structure MUST be less than this value, and all strings contained in this structure MUST fit within the extent defined by this size
        uint32_t LinkInfoSize = 0;

        // A 32-bit, unsigned integer that specifies the size, in bytes, of the LinkInfo header section, which is composed of the LinkInfoSize, LinkInfoHeaderSize, LinkInfoFlags VolumeIDOffset, LocalBasePathOffset, CommonNetworkRelativeLinkOffset, CommonPathSuffixOffset fields, and, if included, the LocalBasePathOffsetUnicode and CommonPathSuffixOffsetUnicode fields
        /*
        0x0000001C              Offsets to the optional fields are not specified.
        0x00000024 ≒ value      Offsets to the optional fields are specified

        In Windows, Unicode characters are stored in this structure if the data cannot be represented as ANSI characters due to truncation of the values. In this case, the value of the LinkInfoHeaderSize field is greater than or equal to 36
        */
        uint32_t LinkInfoHeaderSize = 0;

        // Flags that specify whether the VolumeID, LocalBasePath, LocalBasePathUnicode, and CommonNetworkRelativeLink fields are present in this structure
        LinkInfoFlags LnkInfFlags;

        // A 32-bit, unsigned integer that specifies the location of the VolumeID field. If the VolumeIDAndLocalBasePath flag is set, this value is an offset, in bytes, from the start of the LinkInfo structure; otherwise, this value MUST be zero
        uint32_t VolumeIDOffset = 0;

        // A 32-bit, unsigned integer that specifies the location of the LocalBasePath field. If the VolumeIDAndLocalBasePath flag is set, this value is an offset, in bytes, from the start of the LinkInfo structure; otherwise, this value MUST be zero
        uint32_t LocalBasePathOffset = 0;

        // A 32-bit, unsigned integer that specifies the location of the CommonNetworkRelativeLink field. If the CommonNetworkRelativeLinkAndPathSuffix flag is set, this value is an offset, in bytes, from the start of the LinkInfo structure; otherwise, this value MUST be zero
        uint32_t CommonNetworkRelativeLinkOffset = 0;

        // A 32-bit, unsigned integer that specifies the location of the CommonPathSuffix field. This value is an offset, in bytes, from the start of the LinkInfo structure
        uint32_t CommonPathSuffixOffset = 0;

        // An optional, 32-bit, unsigned integer that specifies the location of the LocalBasePathUnicode field. If the VolumeIDAndLocalBasePath flag is set, this value is an offset, in bytes, from the start of the LinkInfo structure; otherwise, this value MUST be zero. This field can be present only if the value of the LinkInfoHeaderSize field is greater than or equal to 0x00000024
        uint32_t LocalBasePathOffsetUnicode = 0;

        // An optional, 32-bit, unsigned integer that specifies the location of the CommonPathSuffixUnicode field. This value is an offset, in bytes, from the start of the LinkInfo structure. This field can be present only if the value of the LinkInfoHeaderSize field is greater than or equal to 0x00000024
        uint32_t CommonPathSuffixOffsetUnicode = 0;

        // An optional VolumeID structure (section 2.3.1) that specifies information about the volume that the link target was on when the link was created. This field is present if the VolumeIDAndLocalBasePath flag is set
        VolumeID VolID;

        // An optional, NULL每terminated string, defined by the system default code page, which is used to construct the full path to the link item or link target by appending the string in the CommonPathSuffix field. This field is present if the VolumeIDAndLocalBasePath flag is set
        std::string LocalBasePath;

        // An optional CommonNetworkRelativeLink structure (section 2.3.2) that specifies information about the network location where the link target is stored
        CommonNetworkRelativeLink CommonNetRelLnk;

        // A NULL每terminated string, defined by the system default code page, which is used to construct the full path to the link item or link target by being appended to the string in the LocalBasePath field
        std::string CommonPathSuffix;

        // An optional, NULL每terminated, Unicode string that is used to construct the full path to the link item or link target by appending the string in the CommonPathSuffixUnicode field. This field can be present only if the VolumeIDAndLocalBasePath flag is set and the value of the LinkInfoHeaderSize field is greater than or equal to 0x00000024
        std::wstring LocalBasePathUnicode;

        // An optional, NULL每terminated, Unicode string that is used to construct the full path to the link item or link target by being appended to the string in the LocalBasePathUnicode field. This field can be present only if the value of the LinkInfoHeaderSize field is greater than or equal to 0x00000024
        std::wstring CommonPathSuffixUnicode;
    };
#pragma endregion LinkInfo

#pragma region StringData
    struct StringData {
        // REFERED TO AS "Comment". An optional structure that specifies a description of the shortcut that is displayed to end users to identify the purpose of the shell link. This structure MUST be present if the HasName flag is set
        std::wstring NameStringW;
        std::string NameStringA;

        // An optional structure that specifies the location of the link target relative to the file that contains the shell link. When specified, this string SHOULD be used when resolving the link. This structure MUST be present if the HasRelativePath flag is set
        std::wstring RelativePathW;
        std::string RelativePathA;

        // An optional structure that specifies the file system path of the working directory to be used when activating the link target. This structure MUST be present if the HasWorkingDir flag is set
        std::wstring WorkingDirW;
        std::string WorkingDirA;

        // An optional structure that stores the command-line arguments that are specified when activating the link target. This structure MUST be present if the HasArguments flag is set
        std::wstring CommandLineArgumentsW;
        std::string CommandLineArgumentsA;

        // An optional structure that specifies the location of the icon to be used when displaying a shell link item in an icon view. This structure MUST be present if the HasIconLocation flag is set
        std::wstring IconLocationW;
        std::string IconLocationA;
    };
#pragma endregion StringData

#pragma region ExtraData
    enum class ConsoleDataBlockFileAttribute : uint16_t {
        FILA_FOREGROUND_BLUE = 0x0001,       //The foreground text color contains blue.
        FILA_FOREGROUND_GREEN = 0x0002,      //The foreground text color contains green.
        FILA_FOREGROUND_RED = 0x0004,        //The foreground text color contains red.
        FILA_FOREGROUND_INTENSITY = 0x0008,  //The foreground text color is intensified.
        FILA_BACKGROUND_BLUE = 0x0010,       //The background text color contains blue.
        FILA_BACKGROUND_GREEN = 0x0020,      //The background text color contains green.
        FILA_BACKGROUND_RED = 0x0040,        //The background text color contains red.
        FILA_BACKGROUND_INTENSITY = 0x0080   //The background text color is intensified.
    };
    ALLOW_FLAGS_FOR_ENUM_IN_CLASS(ConsoleDataBlockFileAttribute);

    enum class ConsoleDataBlockFontFamily : uint32_t {
        FONTF_FF_DONTCARE = 0x0000,    //The font family is unknown.
        FONTF_FF_ROMAN = 0x0010,       //The font is variable-width with serifs; for example, "Times New Roman".
        FONTF_FF_SWISS = 0x0020,       //The font is variable-width without serifs; for example, "Arial".
        FONTF_FF_MODERN = 0x0030,      //The font is fixed-width, with or without serifs; for example, "Courier New".
        FONTF_FF_SCRIPT = 0x0040,      //The font is designed to look like handwriting; for example, "Cursive".
        FONTF_FF_DECORATIVE = 0x0050,  //The font is a novelty font; for example, "Old English"

        FONTP_TMPF_NONE = 0x0000,         // A font pitch does not apply.
        FONTP_TMPF_FIXED_PITCH = 0x0001,  //The font is a fixed-pitch font.
        FONTP_TMPF_VECTOR = 0x0002,       //The font is a vector font.
        FONTP_TMPF_TRUETYPE = 0x0004,     //The font is a true-type font.
        FONTP_TMPF_DEVICE = 0x0008        //The font is specific to the device.
    };
    ALLOW_FLAGS_FOR_ENUM_IN_CLASS(ConsoleDataBlockFontFamily);

    struct ConsoleDataBlock {
        const uint32_t BlockSize = 0x000000CC;
        static const uint32_t BlockSignature = 0xA0000002;
        /*
                A 16-bit, unsigned integer that specifies the fill attributes that control the
                foreground and background text colors in the console window. The following bit definitions can be
                combined to specify 16 different values each for the foreground and background colors:

                FOREGROUND_BLUE         0x0001      The foreground text color contains blue.
                FOREGROUND_GREEN        0x0002      The foreground text color contains green.
                FOREGROUND_RED          0x0004      The foreground text color contains red.
                FOREGROUND_INTENSITY    0x0008      The foreground text color is intensified.
                BACKGROUND_BLUE         0x0010      The background text color contains blue.
                BACKGROUND_GREEN        0x0020      The background text color contains green.
                BACKGROUND_RED          0x0040      The background text color contains red.
                BACKGROUND_INTENSITY    0x0080      The background text color is intensified.

            */
        ConsoleDataBlockFileAttributes FillAttributes;
        // A 16-bit, unsigned integer that specifies the fill attributes that control the foreground and background text color in the console window popup. The values are the same as for the FillAttributes field
        ConsoleDataBlockFileAttributes PopupFillAttributes;
        // A 16-bit, signed integer that specifies the horizontal size (X axis), in characters, of the console window buffer
        uint16_t ScreenBufferSizeX = 0;
        // A 16-bit, signed integer that specifies the vertical size (Y axis), in characters, of the console window buffer
        uint16_t ScreenBufferSizeY = 0;
        // A 16-bit, signed integer that specifies the horizontal size (X axis), in characters, of the console window
        uint16_t WindowSizeX = 0;
        // A 16-bit, signed integer that specifies the vertical size (Y axis), in characters, of the console window
        uint16_t WindowSizeY = 0;
        // A 16-bit, signed integer that specifies the horizontal coordinate (X axis), in pixels, of the console window origin
        uint16_t WindowOriginX = 0;
        // A 16-bit, signed integer that specifies the vertical coordinate (Y axis), in pixels, of the console window origin
        uint16_t WindowOriginY = 0;
        /*8 bytes unused data*/
        // A 32-bit, unsigned integer that specifies the size, in pixels, of the font used in the console window. The two most significant bytes contain the font height and the two least significant bytes contain the font width. For vector fonts, the width is set to zero
        uint32_t FontSize = 0;
        /*
                A 32-bit, unsigned integer that specifies the family of the font used in the
                console window. This value MUST be comprised of a font family and a font pitch. The values for
                the font family are shown in the following table:
                FF_DONTCARE     0x0000      The font family is unknown.
                FF_ROMAN        0x0010      The font is variable-width with serifs; for example, "Times New Roman".
                FF_SWISS        0x0020      The font is variable-width without serifs; for example, "Arial".
                FF_MODERN       0x0030      The font is fixed-width, with or without serifs; for example, "Courier New".
                FF_SCRIPT       0x0040      The font is designed to look like handwriting; for example, "Cursive".
                FF_DECORATIVE   0x0050      The font is a novelty font; for example, "Old English"

                A bitwise OR of one or more of the following font-pitch bits is added to the font family from the
                previous table:

                TMPF_NONE           0x0000      A font pitch does not apply.
                TMPF_FIXED_PITCH    0x0001      The font is a fixed-pitch font.
                TMPF_VECTOR         0x0002      The font is a vector font.
                TMPF_TRUETYPE       0x0004      The font is a true-type font.
                TMPF_DEVICE         0x0008      The font is specific to the device.
            */
        ConsoleDataBlockFontFamilys FontFamilys;
        // A 32-bit, unsigned integer that specifies the stroke weight of the font used in the console window
        /*
                700 ≒ value     A bold font.
                value < 700     A regular-weight font.
            */
        uint32_t FontWeight = 0;
        // A 32-character Unicode string that specifies the face name of the font used in the console window
        std::wstring FaceName;
        // A 32-bit, unsigned integer that specifies the size of the cursor, in pixels, used in the console window
        /*
                value ≒ 25      A small cursor.
                26 〞 50         A medium cursor.
                51 〞 100        A large cursor.
            */
        uint32_t CursorSize = 0;
        // A 32-bit, unsigned integer that specifies whether to open the console window in full-screen mode
        /*
                0x00000000              Full-screen mode is off.
                0x00000000 < value      Full-screen mode is on.
            */
        uint32_t FullScreen = 0;
        // A 32-bit, unsigned integer that specifies whether to open the console window in QuikEdit mode. In QuickEdit mode, the mouse can be used to cut, copy, and paste text in the console window
        /*
                0x00000000              QuikEdit mode is off.
                0x00000000 < value      QuikEdit mode is on.
            */
        uint32_t QuickEdit = 0;
        // A 32-bit, unsigned integer that specifies insert mode in the console window
        /*
                0x00000000              Insert mode is disabled.
                0x00000000 < value      Insert mode is enabled.
            */
        uint32_t InsertMode = 0;
        // A 32-bit, unsigned integer that specifies auto-position mode of the console window
        /*
                0x00000000              The values of the WindowOriginX and WindowOriginY fields are used to position the console window.
                0x00000000 < value      The console window is positioned automatically
            */
        uint32_t AutoPosition = 0;
        // A 32-bit, unsigned integer that specifies the size, in characters, of the buffer that is used to store a history of user input into the console window
        uint32_t HistoryBufferSize = 0;
        // A 32-bit, unsigned integer that specifies the number of history buffers to use
        uint32_t NumberOfHistoryBuffers = 0;
        // A 32-bit, unsigned integer that specifies whether to remove duplicates in the history buffer
        /*
                0x00000000              Duplicates are not allowed.
                0x00000000 < value      Duplicates are allowed.
            */
        uint32_t HistoryNoDup = 0;
        // A table of 16 32-bit, unsigned integers specifying the RGB colors that are used for text in the console window. The values of the fill attribute fields FillAttributes and PopupFillAttributes are used as indexes into this table to specify the final foreground and background color for a character
        uint32_t ColorTable[16] = {0};
    };

    struct ConsoleFEDataBlock {
        const uint32_t BlockSize = 0x0000000C;
        static const uint32_t BlockSignature = 0xA0000004;
        // A 32-bit, unsigned integer that specifies a code page language code identifier. For details concerning the structure and meaning of language code identifiers, see [MS-LCID]. For additional background information, see [MSCHARSET], [MSDN-CS], and [MSDOCS-CodePage]
        uint32_t CodePage = 0;
    };

    struct DarwinDataBlock {
        const uint32_t BlockSize = 0x00000314;
        static const uint32_t BlockSignature = 0xA0000006;

        // A NULL每terminated string, defined by the system default code page, which specifies an application identifier. This field SHOULD be ignored (260b)
        std::string DarwinDataAnsi;
        // An optional, NULL每terminated, Unicode string that specifies an application identifier (In Windows, this is a Windows Installer (MSI) application descriptor. For more information, see [MSDN-MSISHORTCUTS]) (520b)
        std::wstring DarwinDataUnicode;
    };

    struct EnvironmentVariableDataBlock {
        const uint32_t BlockSize = 0x00000314;
        static const uint32_t BlockSignature = 0xA0000001;

        // A NULL-terminated string, defined by the system default code page, which specifies a path to environment variable information (260b)
        std::string TargetAnsi;
        // An optional, NULL-terminated, Unicode string that specifies a path to environment variable information (520b)
        std::wstring TargetUnicode;
    };

    struct IconEnvironmentDataBlock {
        const uint32_t BlockSize = 0x00000314;
        static const uint32_t BlockSignature = 0xA0000007;
        // A NULL-terminated string, defined by the system default code page, which specifies a path that is constructed with environment variables
        std::string TargetAnsi;
        // An optional, NULL-terminated, Unicode string that specifies a path that is constructed with environment variables
        std::wstring TargetUnicode;
    };

    struct KnownFolderDataBlock {
        const uint32_t BlockSize = 0x0000001C;
        static const uint32_t BlockSignature = 0xA000000B;
        // A value in GUID packet representation ([MS-DTYP] section 2.3.4.2) that specifies the folder GUID ID
        std::string KnownFolderID;
        // A 32-bit, unsigned integer that specifies the location of the ItemID of the first child segment of the IDList specified by KnownFolderID. This value is the offset, in bytes, into the link target IDList
        uint32_t Offset = 0;
    };

    struct PropertyStoreDataBlock {
        // BlockSize MUST be >= 0x0000000C
        uint32_t BlockSize;
        static const uint32_t BlockSignature = 0xA0000009;
        // A serialized property storage structure ([MS-PROPSTORE] section 2.2)
        std::string PropertyStore;
    };

    struct ShimDataBlock {
        // BlockSize MUST be >= 0x00000088
        uint32_t BlockSize = 0;
        static const uint32_t BlockSignature = 0xA0000008;
        // A Unicode string that specifies the name of a shim layer to apply to a link target when it is being activated
        std::wstring LayerName;
    };

    struct SpecialFolderDataBlock {
        const uint32_t BlockSize = 0x00000010;
        static const uint32_t BlockSignature = 0xA0000005;
        // A 32-bit, unsigned integer that specifies the folder integer ID
        uint32_t SpecialFolderID = 0;
        // A 32-bit, unsigned integer that specifies the location of the ItemID of the first child segment of the IDList specified by SpecialFolderID. This value is the offset, in bytes, into the link target IDList
        uint32_t Offset = 0;
    };

    struct TrackerDataBlock {
        const uint32_t BlockSize = 0x00000060;
        static const uint32_t BlockSignature = 0xA0000003;
        // A 32-bit, unsigned integer that specifies the size of the rest of the TrackerDataBlock structure, including this Length field. This value MUST be 0x00000058
        uint32_t Length = 0;
        // A 32-bit, unsigned integer. This value MUST be 0x00000000
        uint32_t Version = 0;
        // A NULL每terminated character string, as defined by the system default code page, which specifies the NetBIOS name of the machine where the link target was last known to reside
        std::string MachineID;
        // Two values in GUID packet representation ([MS-DTYP] section 2.3.4.2) that are used to find the link target with the Link Tracking service, as described in [MS-DLTW]
        std::string Droid;
        // Two values in GUID packet representation that are used to find the link target with the Link Tracking service
        std::string DroidBirth;
    };

    struct VistaAndAboveIDListDataBlock {
        // BlockSize MUST be >= 0x0000000A
        uint32_t BlockSize = 0;
        static const uint32_t BlockSignature = 0xA000000C;
        // An IDList structure (section 2.2.1)
        LinkTargetIDList targetIdList;
    };

    struct ExtraData {
        ConsoleDataBlock consoleDB;
        ConsoleFEDataBlock consoleFEDB;
        DarwinDataBlock darDB;
        EnvironmentVariableDataBlock envVarDB;
        IconEnvironmentDataBlock iconEnvDB;
        KnownFolderDataBlock knownFolderDB;
        PropertyStoreDataBlock propStoreDB;
        ShimDataBlock shimDB;
        SpecialFolderDataBlock speFolderDB;
        TrackerDataBlock trackerDB;
        VistaAndAboveIDListDataBlock vistaAboveIDListDB;
    };
#pragma endregion ExtraData

    WinShellink();
    virtual ~WinShellink();

    ShellinkErr load(const std::wstring& lnkPath);
    bool saveAs(const std::wstring& lnkPath);

    LinkHeader linkHeader() const { return header_; }
    LinkTargetIDList linkTargetIdList() const { return targetIdList_; }
    LinkInfo linkInfo() const { return linkInfo_; }
    StringData stringData() const { return stringData_; }
    ExtraData extraData() const { return extraData_; }

    std::wstring getDescription() const;
    std::wstring getTargetPath() const;
    std::wstring getArguments() const;
    std::wstring getIconPath() const;
    int32_t getIconIndex() const;

    // such as @%SystemRoot%\\system32\\%SystemRoot%.dll,-10113
    static bool IsResourceString(const std::wstring& s);

    // Notice: 32bit program can not load 64bit dll
    static bool LoadStringFromRes(const std::wstring& resStr, std::wstring& result);

   private:
    WinShellink::ShellinkErr WinShellink::readEConsoleDataBlock(uint32_t blockSize, uint32_t blockSignature, FILE* fp);
    WinShellink::ShellinkErr WinShellink::readEConsoleFEDataBlock(uint32_t blockSize, uint32_t blockSignature, FILE* fp);
    WinShellink::ShellinkErr WinShellink::readEDarwinDataBlock(uint32_t blockSize, uint32_t blockSignature, FILE* fp);
    WinShellink::ShellinkErr WinShellink::readEEnvironmentVariableDataBlock(uint32_t blockSize, uint32_t blockSignature, FILE* fp);
    WinShellink::ShellinkErr WinShellink::readEIconEnvironmentDataBlock(uint32_t blockSize, uint32_t blockSignature, FILE* fp);
    WinShellink::ShellinkErr WinShellink::readEKnownFolderDataBlock(uint32_t blockSize, uint32_t blockSignature, FILE* fp);
    WinShellink::ShellinkErr WinShellink::readEPropertyStoreDataBlock(uint32_t blockSize, uint32_t blockSignature, FILE* fp);
    WinShellink::ShellinkErr WinShellink::readEShimDataBlock(uint32_t blockSize, uint32_t blockSignature, FILE* fp);
    WinShellink::ShellinkErr WinShellink::readESpecialFolderDataBlock(uint32_t blockSize, uint32_t blockSignature, FILE* fp);
    WinShellink::ShellinkErr WinShellink::readETrackerDataBlock(uint32_t blockSize, uint32_t blockSignature, FILE* fp);
    WinShellink::ShellinkErr WinShellink::readEVistaAndAboveIDListDataBlock(uint32_t blockSize, uint32_t blockSignature, FILE* fp);

    // read NULL terminated String
    ShellinkErr readNULLStr(char** dest, ShellinkErr errv1, ShellinkErr errv2, FILE* fp);
    ShellinkErr readNULLWStr(wchar_t** dest, ShellinkErr errv1, ShellinkErr errv2, FILE* fp);
    ShellinkErr readNULLStr(std::vector<char>& dest, ShellinkErr errv1, ShellinkErr errv2, FILE* fp);
    ShellinkErr readNULLWStr(std::vector<wchar_t>& dest, ShellinkErr errv1, ShellinkErr errv2, FILE* fp);

    // read special size string
    ShellinkErr readStr(std::string& dest, ShellinkErr errv1, ShellinkErr errv2, FILE* fp, size_t size);
    ShellinkErr readWStr(std::wstring& dest, ShellinkErr errv1, ShellinkErr errv2, FILE* fp, size_t size);

    //Converts little Endian to big Endian and vice versa (size in bytes)
    void toBigEndian(void* inp, size_t size);

   private:
    LinkHeader header_;
    LinkTargetIDList targetIdList_;
    LinkInfo linkInfo_;
    StringData stringData_;
    ExtraData extraData_;

    const int kExtraDataBlockNum = 11;
};
}  // namespace jhc

#ifndef JHC_NOT_HEADER_ONLY
#include "impl/win_shellink.cc"
#endif
#endif
#endif  // !JHC_WIN_SHELLINK_HPP__