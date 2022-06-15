#include "jhc/config.hpp"
#ifdef JHC_NOT_HEADER_ONLY
#include "../win_shellink.hpp"
#endif

#ifdef JHC_WIN
#include "jhc/scoped_object.hpp"
#include "jhc/string_encode.hpp"
#include "jhc/string_helper.hpp"
#include "jhc/path_util.hpp"
#include <shlobj_core.h>

#pragma comment(lib, "Shell32.lib")

namespace jhc {

WinShellink::WinShellink() {
}

WinShellink::~WinShellink() {
}

WinShellink::ShellinkErr WinShellink::load(const std::wstring& lnkPath) {
    FILE* fp = nullptr;
    _wfopen_s(&fp, lnkPath.c_str(), L"rb");

    if (!fp)
        return ShellinkErr::SHLLINK_ERR_FCL;

    ScopedFile scopedFile(fp);

    WinShellink::ShellinkErr err;
    // HEADER
    {
        //HeaderSize
        uint32_t headerSizeValue = 0;
        if (fread(&headerSizeValue, 4, 1, fp) != 1)
            return ShellinkErr::SHLLINK_ERR_FIO;

        if (header_.HeaderSize != headerSizeValue)
            return ShellinkErr::SHLLINK_ERR_WHEADS;

        //LinkCLSID
        if (fread(&header_.LinkCLSID_L, 16, 1, fp) != 1)
            return ShellinkErr::SHLLINK_ERR_FIO;
        toBigEndian(&header_.LinkCLSID_L, 16);
        if (header_.LinkCLSID_H != 0x0114020000000000)
            return ShellinkErr::SHLLINK_ERR_WCLSIDS;
        if (header_.LinkCLSID_L != 0xC000000000000046)
            return ShellinkErr::SHLLINK_ERR_WCLSIDS;

        //LinkFlags
        uint32_t linkFlagsValue = 0;
        if (fread(&linkFlagsValue, 4, 1, fp) != 1)
            return ShellinkErr::SHLLINK_ERR_FIO;
        header_.LinkFlags.set_underlying_value(linkFlagsValue);

        //FileAttributes
        uint32_t fileAttributesValue = 0;
        if (fread(&fileAttributesValue, 4, 1, fp) != 1)
            return ShellinkErr::SHLLINK_ERR_FIO;
        header_.TargetFileAttributes.set_underlying_value(fileAttributesValue);

        //CreationTime
        if (fread(&header_.CreationTime, 8, 1, fp) != 1)
            return ShellinkErr::SHLLINK_ERR_FIO;
        //AccessTime
        if (fread(&header_.AccessTime, 8, 1, fp) != 1)
            return ShellinkErr::SHLLINK_ERR_FIO;
        //WriteTime
        if (fread(&header_.WriteTime, 8, 1, fp) != 1)
            return ShellinkErr::SHLLINK_ERR_FIO;

        //FileSize
        if (fread(&header_.FileSize, 4, 1, fp) != 1)
            return ShellinkErr::SHLLINK_ERR_FIO;

        //IconIndex
        if (fread(&header_.IconIndex, 4, 1, fp) != 1)
            return ShellinkErr::SHLLINK_ERR_FIO;

        //ShowCommand
        if (fread(&header_.ShowCommand, 4, 1, fp) != 1)
            return ShellinkErr::SHLLINK_ERR_FIO;

        //HotKey
        if (fread(&header_.HotKey, 2, 1, fp) != 1)
            return ShellinkErr::SHLLINK_ERR_FIO;

        fseek(fp, 10, SEEK_CUR);
    }

    // LinkTargetIDList
    if (header_.LinkFlags & ShllinkLinkFlag::LF_HasLinkTargetIDList) {
        //IDList size
        if (fread(&targetIdList_.idListSize, 2, 1, fp) != 1)
            return ShellinkErr::SHLLINK_ERR_FIO;

        targetIdList_.IDListData.resize(targetIdList_.idListSize);
        if (fread((void*)(&targetIdList_.IDListData[0]), targetIdList_.idListSize, 1, fp) != 1)
            return ShellinkErr::SHLLINK_ERR_FIO;

#if 0
        int tmpS = targetIdList_.idListSize - 2;  // TerminalID (2 bytes)
        while (tmpS > 0) {
            //Element size
            uint16_t itemSize = 0;
            if (fread(&itemSize, 2, 1, fp) != 1)
                return ShellinkErr::SHLLINK_ERR_FIO;

            //Element (size -2 as struct contains one 2byte var)
            uint16_t itemDataSize = itemSize - 2;
            std::vector<uint8_t> vItemId((size_t)(itemDataSize));

            if (itemDataSize > 0) {
                if (fread((void*)(&vItemId[0]), itemDataSize, 1, fp) != 1)
                    return ShellinkErr::SHLLINK_ERR_FIO;
            }
            tmpS -= itemSize;
            targetIdList_.itemIDList.push_back(vItemId);
        }

        uint16_t nullb;
        if (fread(&nullb, 2, 1, fp) != 1)
            return ShellinkErr::SHLLINK_ERR_FIO;

        if (tmpS < 0 || nullb != 0)
            return ShellinkErr::SHLLINK_ERR_INVIDL;
#endif
    }

    char* pValue = nullptr;
    wchar_t* pValueW = nullptr;
    // LinkInfo
    if (header_.LinkFlags & ShllinkLinkFlag::LF_HasLinkInfo) {
        //LinkInfoSize
        if (fread(&linkInfo_.LinkInfoSize, 4, 1, fp) != 1)
            return ShellinkErr::SHLLINK_ERR_FIO;

        //LinkInfoHeaderSize
        if (fread(&linkInfo_.LinkInfoHeaderSize, 4, 1, fp) != 1)
            return ShellinkErr::SHLLINK_ERR_FIO;
        if (linkInfo_.LinkInfoHeaderSize != 0x0000001C && linkInfo_.LinkInfoHeaderSize < 0x00000024)
            return ShellinkErr::SHLLINK_ERR_INVLIHS;

        //LinkInfoFlags
        uint32_t linkInfoFlagsValue = 0;
        if (fread(&linkInfoFlagsValue, 4, 1, fp) != 1)
            return ShellinkErr::SHLLINK_ERR_FIO;
        linkInfo_.LnkInfFlags.set_underlying_value(linkInfoFlagsValue);

        //VolumeIDOffset
        if (fread(&linkInfo_.VolumeIDOffset, 4, 1, fp) != 1)
            return ShellinkErr::SHLLINK_ERR_FIO;

        //LocalBasePathOffset
        if (fread(&linkInfo_.LocalBasePathOffset, 4, 1, fp) != 1)
            return ShellinkErr::SHLLINK_ERR_FIO;

        //CommonNetworkRelativeLinkOffset
        if (fread(&linkInfo_.CommonNetworkRelativeLinkOffset, 4, 1, fp) != 1)
            return ShellinkErr::SHLLINK_ERR_FIO;

        //CommonPathSuffixOffset
        if (fread(&linkInfo_.CommonPathSuffixOffset, 4, 1, fp) != 1)
            return ShellinkErr::SHLLINK_ERR_FIO;

        //only present if LinkInfoHeaderSize >= 0x00000024
        if (linkInfo_.LinkInfoHeaderSize >= 0x00000024) {
            //LocalBasePathOffsetUnicode
            if (fread(&linkInfo_.LocalBasePathOffsetUnicode, 4, 1, fp) != 1)
                return ShellinkErr::SHLLINK_ERR_FIO;

            //CommonPathSuffixOffsetUnicode
            if (fread(&linkInfo_.CommonPathSuffixOffsetUnicode, 4, 1, fp) != 1)
                return ShellinkErr::SHLLINK_ERR_FIO;
        }

        // VolumeID
        if (linkInfo_.LnkInfFlags & LinkInfoFlag::LIF_VolumeIDAndLocalBasePath) {
            int VtmpSize = 16;
            //VolumeIDSize
            if (fread(&linkInfo_.VolID.VolumeIDSize, 4, 1, fp) != 1)
                return ShellinkErr::SHLLINK_ERR_FIO;

            //DriveType
            if (fread(&linkInfo_.VolID.DriveType, 4, 1, fp) != 1)
                return ShellinkErr::SHLLINK_ERR_FIO;

            //DriveSerialNumber
            if (fread(&linkInfo_.VolID.DriveSerialNumber, 4, 1, fp) != 1)
                return ShellinkErr::SHLLINK_ERR_FIO;

            //VolumeLabelOffset
            if (fread(&linkInfo_.VolID.VolumeLabelOffset, 4, 1, fp) != 1)
                return ShellinkErr::SHLLINK_ERR_FIO;

            //only present if VolumeLabelOffset == 0x00000014
            if (linkInfo_.VolID.VolumeLabelOffset == 0x00000014) {
                //VolumeLabelOffsetUnicode
                if (fread(&linkInfo_.VolID.VolumeLabelOffsetUnicode, 4, 1, fp) != 1)
                    return ShellinkErr::SHLLINK_ERR_FIO;
                VtmpSize += 4;
            }

            //DATA
            if (linkInfo_.VolID.VolumeIDSize <= VtmpSize)
                return ShellinkErr::SHLLINK_ERR_VIDSLOW;
            linkInfo_.VolID.Data.resize(linkInfo_.VolID.VolumeIDSize - VtmpSize);
            if (linkInfo_.VolID.Data.size() != linkInfo_.VolID.VolumeIDSize - VtmpSize)
                return ShellinkErr::SHLLINK_ERR_NULLPVIDD;
            //Data
            if (linkInfo_.VolID.VolumeLabelOffset == 0x00000014) {
                if (fread(&linkInfo_.VolID.Data[0], sizeof(wchar_t), (linkInfo_.VolID.VolumeIDSize - VtmpSize) / 2, fp) != (linkInfo_.VolID.VolumeIDSize - VtmpSize) / 2)
                    return ShellinkErr::SHLLINK_ERR_FIO;
            }
            else {
                if (fread(&linkInfo_.VolID.Data[0], sizeof(char), linkInfo_.VolID.VolumeIDSize - VtmpSize, fp) != linkInfo_.VolID.VolumeIDSize - VtmpSize)
                    return ShellinkErr::SHLLINK_ERR_FIO;
            }

            //LocalBasePath
            pValue = nullptr;
            if ((err = readNULLStr(&pValue, ShellinkErr::SHLLINK_ERR_NULLPLBP, ShellinkErr::SHLLINK_ERR_FIO, fp)) != ShellinkErr::SHLLINK_ERR_NONE)
                return err;
            linkInfo_.LocalBasePath = pValue;
            free(pValue);
        }

        // CommonNetworkRelativeLink
        if (linkInfo_.LnkInfFlags & LinkInfoFlag::LIF_CommonNetworkRelativeLinkAndPathSuffix) {
            //CommonNetworkRelativeSize
            if (fread(&linkInfo_.CommonNetRelLnk.CommonNetworkRelativeSize, 4, 1, fp) != 1)
                return ShellinkErr::SHLLINK_ERR_FIO;

            //CommonNetworkRelativeLinkFlags
            uint32_t commonNetworkRelativeLinkFlagsValue = 0;
            if (fread(&commonNetworkRelativeLinkFlagsValue, 4, 1, fp) != 1)
                return ShellinkErr::SHLLINK_ERR_FIO;
            linkInfo_.CommonNetRelLnk.ComNetRelLnkFlags.set_underlying_value(commonNetworkRelativeLinkFlagsValue);

            //NetNameOffset
            if (fread(&linkInfo_.CommonNetRelLnk.NetNameOffset, 4, 1, fp) != 1)
                return ShellinkErr::SHLLINK_ERR_FIO;

            //DeviceNameOffset
            if (fread(&linkInfo_.CommonNetRelLnk.DeviceNameOffset, 4, 1, fp) != 1)
                return ShellinkErr::SHLLINK_ERR_FIO;

            //NetworkProviderType
            if (fread(&linkInfo_.CommonNetRelLnk.NetworkProviderType, 4, 1, fp) != 1)
                return ShellinkErr::SHLLINK_ERR_FIO;

            if (linkInfo_.CommonNetRelLnk.NetNameOffset > 0x00000014) {
                //NetNameOffsetUnicode
                if (fread(&linkInfo_.CommonNetRelLnk.NetNameOffsetUnicode, 4, 1, fp) != 1)
                    return ShellinkErr::SHLLINK_ERR_FIO;

                //DeviceNameOffsetUnicode
                if (fread(&linkInfo_.CommonNetRelLnk.DeviceNameOffsetUnicode, 4, 1, fp) != 1)
                    return ShellinkErr::SHLLINK_ERR_FIO;
            }

            //NetName
            pValue = nullptr;
            if ((err = readNULLStr(&pValue, ShellinkErr::SHLLINK_ERR_NULLPNETN, ShellinkErr::SHLLINK_ERR_FIO, fp)) != ShellinkErr::SHLLINK_ERR_NONE)
                return err;
            linkInfo_.CommonNetRelLnk.NetName = pValue;
            free(pValue);

            //DeviceName
            pValue = nullptr;
            if ((err = readNULLStr(&pValue, ShellinkErr::SHLLINK_ERR_NULLPDEVN, ShellinkErr::SHLLINK_ERR_FIO, fp)) != ShellinkErr::SHLLINK_ERR_NONE)
                return err;
            linkInfo_.CommonNetRelLnk.DeviceName = pValue;
            free(pValue);

            if (linkInfo_.CommonNetRelLnk.NetNameOffset > 0x00000014) {
                pValueW = nullptr;
                //NetNameUnicode
                if ((err = readNULLWStr(&pValueW, ShellinkErr::SHLLINK_ERR_NULLPNNU, ShellinkErr::SHLLINK_ERR_FIO, fp)) != ShellinkErr::SHLLINK_ERR_NONE)
                    return err;
                linkInfo_.CommonNetRelLnk.NetNameUnicode = pValueW;
                free(pValueW);

                //DeviceNameUnicode
                pValueW = nullptr;
                if ((err = readNULLWStr(&pValueW, ShellinkErr::SHLLINK_ERR_NULLPDNU, ShellinkErr::SHLLINK_ERR_FIO, fp)) != ShellinkErr::SHLLINK_ERR_NONE)
                    return err;
                linkInfo_.CommonNetRelLnk.DeviceNameUnicode = pValueW;
                free(pValueW);
            }
        }

        //CommonPathSuffix
        pValue = nullptr;
        if ((err = readNULLStr(&pValue, ShellinkErr::SHLLINK_ERR_NULLPCPS, ShellinkErr::SHLLINK_ERR_FIO, fp)) != ShellinkErr::SHLLINK_ERR_NONE)
            return err;
        linkInfo_.CommonPathSuffix = pValue;
        free(pValue);

        if (linkInfo_.LinkInfoHeaderSize >= 0x00000024) {
            pValueW = nullptr;

            //LocalBasePathUnicode
            if (linkInfo_.LnkInfFlags & LinkInfoFlag::LIF_VolumeIDAndLocalBasePath) {
                if ((err = readNULLWStr(&pValueW, ShellinkErr::SHLLINK_ERR_NULLPLBPU, ShellinkErr::SHLLINK_ERR_FIO, fp)) != ShellinkErr::SHLLINK_ERR_NONE)
                    return err;
                linkInfo_.LocalBasePathUnicode = pValueW;
                free(pValueW);
            }

            //CommonPathSuffixUnicode
            if ((err = readNULLWStr(&pValueW, ShellinkErr::SHLLINK_ERR_NULLPCPSU, ShellinkErr::SHLLINK_ERR_FIO, fp)) != ShellinkErr::SHLLINK_ERR_NONE)
                return err;
            linkInfo_.CommonPathSuffixUnicode = pValueW;
            free(pValueW);
        }
    }

    // StringData (all unicode 2 bytes)
    if (header_.LinkFlags & ShllinkLinkFlag::LF_HasName) {
        //NameString
        uint16_t countCharacters = 0;
        if (fread(&countCharacters, 2, 1, fp) != 1)
            return ShellinkErr::SHLLINK_ERR_FIO;

        if (header_.LinkFlags & ShllinkLinkFlag::LF_IsUnicode) {
            if ((err = readWStr(stringData_.NameStringW, ShellinkErr::SHLLINK_ERR_NULLPSTRDNAME, ShellinkErr::SHLLINK_ERR_FIO, fp, countCharacters * 2)) != ShellinkErr::SHLLINK_ERR_NONE)
                return err;
        }
        else {
            if ((err = readStr(stringData_.NameStringA, ShellinkErr::SHLLINK_ERR_NULLPSTRDNAME, ShellinkErr::SHLLINK_ERR_FIO, fp, countCharacters)) != ShellinkErr::SHLLINK_ERR_NONE)
                return err;
        }
    }
    if (header_.LinkFlags & ShllinkLinkFlag::LF_HasRelativePath) {
        //RelativePath
        uint16_t countCharacters = 0;
        if (fread(&countCharacters, 2, 1, fp) != 1)
            return ShellinkErr::SHLLINK_ERR_FIO;

        if (header_.LinkFlags & ShllinkLinkFlag::LF_IsUnicode) {
            if ((err = readWStr(stringData_.RelativePathW, ShellinkErr::SHLLINK_ERR_NULLPSTRDRPATH, ShellinkErr::SHLLINK_ERR_FIO, fp, countCharacters * 2)) != ShellinkErr::SHLLINK_ERR_NONE)
                return err;
        }
        else {
            if ((err = readStr(stringData_.RelativePathA, ShellinkErr::SHLLINK_ERR_NULLPSTRDRPATH, ShellinkErr::SHLLINK_ERR_FIO, fp, countCharacters)) != ShellinkErr::SHLLINK_ERR_NONE)
                return err;
        }
    }
    if (header_.LinkFlags & ShllinkLinkFlag::LF_HasWorkingDir) {
        //WorkingDir
        uint16_t countCharacters = 0;
        if (fread(&countCharacters, 2, 1, fp) != 1)
            return ShellinkErr::SHLLINK_ERR_FIO;

        if (header_.LinkFlags & ShllinkLinkFlag::LF_IsUnicode) {
            if ((err = readWStr(stringData_.WorkingDirW, ShellinkErr::SHLLINK_ERR_NULLPSTRDWDIR, ShellinkErr::SHLLINK_ERR_FIO, fp, countCharacters * 2)) != ShellinkErr::SHLLINK_ERR_NONE)
                return err;
        }
        else {
            if ((err = readStr(stringData_.WorkingDirA, ShellinkErr::SHLLINK_ERR_NULLPSTRDWDIR, ShellinkErr::SHLLINK_ERR_FIO, fp, countCharacters)) != ShellinkErr::SHLLINK_ERR_NONE)
                return err;
        }
    }
    if (header_.LinkFlags & ShllinkLinkFlag::LF_HasArguments) {
        //CommandLineArguments
        uint16_t countCharacters = 0;
        if (fread(&countCharacters, 2, 1, fp) != 1)
            return ShellinkErr::SHLLINK_ERR_FIO;

        if (header_.LinkFlags & ShllinkLinkFlag::LF_IsUnicode) {
            if ((err = readWStr(stringData_.CommandLineArgumentsW, ShellinkErr::SHLLINK_ERR_NULLPSTRDARG, ShellinkErr::SHLLINK_ERR_FIO, fp, countCharacters * 2)) != ShellinkErr::SHLLINK_ERR_NONE)
                return err;
        }
        else {
            if ((err = readStr(stringData_.CommandLineArgumentsA, ShellinkErr::SHLLINK_ERR_NULLPSTRDARG, ShellinkErr::SHLLINK_ERR_FIO, fp, countCharacters)) != ShellinkErr::SHLLINK_ERR_NONE)
                return err;
        }
    }
    if (header_.LinkFlags & ShllinkLinkFlag::LF_HasIconLocation) {
        //IconLocation
        uint16_t countCharacters = 0;
        if (fread(&countCharacters, 2, 1, fp) != 1)
            return ShellinkErr::SHLLINK_ERR_FIO;

        if (header_.LinkFlags & ShllinkLinkFlag::LF_IsUnicode) {
            if ((err = readWStr(stringData_.IconLocationW, ShellinkErr::SHLLINK_ERR_NULLPSTRDICO, ShellinkErr::SHLLINK_ERR_FIO, fp, countCharacters * 2)) != ShellinkErr::SHLLINK_ERR_NONE)
                return err;
        }
        else {
            if ((err = readStr(stringData_.IconLocationA, ShellinkErr::SHLLINK_ERR_NULLPSTRDICO, ShellinkErr::SHLLINK_ERR_FIO, fp, countCharacters)) != ShellinkErr::SHLLINK_ERR_NONE)
                return err;
        }
    }

    // ExtraDataBlock
    {
        long int cpos = ftell(fp);
        fseek(fp, 0, SEEK_END);
        long int epos = ftell(fp) - 4;  //TerminalBlock
        fseek(fp, cpos, SEEK_SET);

        for (int i = 0; i < kExtraDataBlockNum; i++) {
            cpos = ftell(fp);
            if (cpos >= epos)
                break;

            uint32_t blockSize = 0;
            uint32_t blockSignature = 0;

            if (fread(&blockSize, 4, 1, fp) != 1)
                return ShellinkErr::SHLLINK_ERR_FIO;

            if (fread(&blockSignature, 4, 1, fp) != 1)
                return ShellinkErr::SHLLINK_ERR_FIO;

            switch (blockSignature) {
                case ConsoleDataBlock::BlockSignature:
                    if ((err = readEConsoleDataBlock(blockSize, blockSignature, fp)) != ShellinkErr::SHLLINK_ERR_NONE) {
                        return err;
                    }
                    break;
                case ConsoleFEDataBlock::BlockSignature:
                    if ((err = readEConsoleFEDataBlock(blockSize, blockSignature, fp)) != ShellinkErr::SHLLINK_ERR_NONE) {
                        return err;
                    }
                    break;
                case DarwinDataBlock::BlockSignature:
                    if ((err = readEDarwinDataBlock(blockSize, blockSignature, fp)) != ShellinkErr::SHLLINK_ERR_NONE) {
                        return err;
                    }
                    break;
                case EnvironmentVariableDataBlock::BlockSignature:
                    if ((err = readEEnvironmentVariableDataBlock(blockSize, blockSignature, fp)) != ShellinkErr::SHLLINK_ERR_NONE) {
                        return err;
                    }
                    break;
                case IconEnvironmentDataBlock::BlockSignature:
                    if ((err = readEIconEnvironmentDataBlock(blockSize, blockSignature, fp)) != ShellinkErr::SHLLINK_ERR_NONE) {
                        return err;
                    }
                    break;
                case KnownFolderDataBlock::BlockSignature:
                    if ((err = readEKnownFolderDataBlock(blockSize, blockSignature, fp)) != ShellinkErr::SHLLINK_ERR_NONE) {
                        return err;
                    }
                    break;
                case PropertyStoreDataBlock::BlockSignature:
                    if ((err = readEPropertyStoreDataBlock(blockSize, blockSignature, fp)) != ShellinkErr::SHLLINK_ERR_NONE) {
                        return err;
                    }
                    break;
                case ShimDataBlock::BlockSignature:
                    if ((err = readEShimDataBlock(blockSize, blockSignature, fp)) != ShellinkErr::SHLLINK_ERR_NONE) {
                        return err;
                    }
                    break;
                case SpecialFolderDataBlock::BlockSignature:
                    if ((err = readESpecialFolderDataBlock(blockSize, blockSignature, fp)) != ShellinkErr::SHLLINK_ERR_NONE) {
                        return err;
                    }
                    break;
                case TrackerDataBlock::BlockSignature:
                    if ((err = readETrackerDataBlock(blockSize, blockSignature, fp)) != ShellinkErr::SHLLINK_ERR_NONE) {
                        return err;
                    }
                    break;
                case VistaAndAboveIDListDataBlock::BlockSignature:
                    if ((err = readEVistaAndAboveIDListDataBlock(blockSize, blockSignature, fp)) != ShellinkErr::SHLLINK_ERR_NONE) {
                        return err;
                    }
                    break;
                default:
                    return ShellinkErr::SHLLINK_ERR_UNKEDBSIG;
            }
        }
    }

    return ShellinkErr::SHLLINK_ERR_NONE;
}

WinShellink::ShellinkErr WinShellink::readEConsoleDataBlock(uint32_t blockSize, uint32_t blockSignature, FILE* fp) {
    WinShellink::ShellinkErr err;
    if (blockSize != extraData_.consoleDB.BlockSize)
        return (ShellinkErr::SHLLINK_ERRX_WRONGSIZE);

    //FillAttributes
    uint16_t fileAttributesValue = 0;
    if (fread(&fileAttributesValue, 2, 1, fp) != 1)
        return (ShellinkErr::SHLLINK_ERR_FIO);
    extraData_.consoleDB.FillAttributes.set_underlying_value(fileAttributesValue);

    //PopupFillAttributes
    uint16_t popupFileAttributesValue = 0;
    if (fread(&popupFileAttributesValue, 2, 1, fp) != 1)
        return (ShellinkErr::SHLLINK_ERR_FIO);
    extraData_.consoleDB.PopupFillAttributes.set_underlying_value(popupFileAttributesValue);

    //ScreenBufferSizeX
    if (fread(&extraData_.consoleDB.ScreenBufferSizeX, 2, 1, fp) != 1)
        return (ShellinkErr::SHLLINK_ERR_FIO);

    //ScreenBufferSizeY
    if (fread(&extraData_.consoleDB.ScreenBufferSizeY, 2, 1, fp) != 1)
        return (ShellinkErr::SHLLINK_ERR_FIO);

    //WindowSizeX
    if (fread(&extraData_.consoleDB.WindowSizeX, 2, 1, fp) != 1)
        return (ShellinkErr::SHLLINK_ERR_FIO);

    //WindowSizeY
    if (fread(&extraData_.consoleDB.WindowSizeY, 2, 1, fp) != 1)
        return (ShellinkErr::SHLLINK_ERR_FIO);

    //WindowOriginX
    if (fread(&extraData_.consoleDB.WindowOriginX, 2, 1, fp) != 1)
        return (ShellinkErr::SHLLINK_ERR_FIO);

    //WindowOriginY
    if (fread(&extraData_.consoleDB.WindowOriginY, 2, 1, fp) != 1)
        return (ShellinkErr::SHLLINK_ERR_FIO);

    fseek(fp, 8, SEEK_CUR);

    //FontSize
    if (fread(&extraData_.consoleDB.FontSize, 4, 1, fp) != 1)
        return (ShellinkErr::SHLLINK_ERR_FIO);

    //FontFamily
    uint32_t fontFamilysValue = 0;
    if (fread(&fontFamilysValue, 4, 1, fp) != 1)
        return (ShellinkErr::SHLLINK_ERR_FIO);
    extraData_.consoleDB.FontFamilys.set_underlying_value(fontFamilysValue);

    //FontWeight
    if (fread(&extraData_.consoleDB.FontWeight, 4, 1, fp) != 1)
        return (ShellinkErr::SHLLINK_ERR_FIO);

    //FaceName
    if ((err = readWStr(extraData_.consoleDB.FaceName, ShellinkErr::SHLLINK_ERRX_NULLPSTRFNAME, ShellinkErr::SHLLINK_ERR_FIO, fp, 32)) != ShellinkErr::SHLLINK_ERR_NONE)
        return err;

    //CursorSize
    if (fread(&extraData_.consoleDB.CursorSize, 4, 1, fp) != 1)
        return (ShellinkErr::SHLLINK_ERR_FIO);

    //FullScreen
    if (fread(&extraData_.consoleDB.FullScreen, 4, 1, fp) != 1)
        return (ShellinkErr::SHLLINK_ERR_FIO);

    //QuickEdit
    if (fread(&extraData_.consoleDB.QuickEdit, 4, 1, fp) != 1)
        return (ShellinkErr::SHLLINK_ERR_FIO);

    //InsertMode
    if (fread(&extraData_.consoleDB.InsertMode, 4, 1, fp) != 1)
        return (ShellinkErr::SHLLINK_ERR_FIO);

    //AutoPosition
    if (fread(&extraData_.consoleDB.AutoPosition, 4, 1, fp) != 1)
        return (ShellinkErr::SHLLINK_ERR_FIO);

    //HistoryBufferSize
    if (fread(&extraData_.consoleDB.HistoryBufferSize, 4, 1, fp) != 1)
        return (ShellinkErr::SHLLINK_ERR_FIO);

    //NumberOfHistoryBuffers
    if (fread(&extraData_.consoleDB.NumberOfHistoryBuffers, 4, 1, fp) != 1)
        return (ShellinkErr::SHLLINK_ERR_FIO);

    //HistoryNoDup
    if (fread(&extraData_.consoleDB.HistoryNoDup, 4, 1, fp) != 1)
        return (ShellinkErr::SHLLINK_ERR_FIO);

    //AutoPosition
    if (fread(&extraData_.consoleDB.AutoPosition, 4, 1, fp) != 1)
        return (ShellinkErr::SHLLINK_ERR_FIO);

    //ColorTable
    for (int i = 0; i < 16; i++)
        if (fread(&extraData_.consoleDB.ColorTable[i], 4, 1, fp) != 1)
            return (ShellinkErr::SHLLINK_ERR_FIO);

    return ShellinkErr::SHLLINK_ERR_NONE;
}

WinShellink::ShellinkErr WinShellink::readEConsoleFEDataBlock(uint32_t blockSize, uint32_t blockSignature, FILE* fp) {
    if (blockSize != extraData_.consoleFEDB.BlockSize)
        return (ShellinkErr::SHLLINK_ERRX_WRONGSIZE);

    //CodePage
    if (fread(&extraData_.consoleFEDB.CodePage, 4, 1, fp) != 1)
        return (ShellinkErr::SHLLINK_ERR_FIO);

    return ShellinkErr::SHLLINK_ERR_NONE;
}

WinShellink::ShellinkErr WinShellink::readEDarwinDataBlock(uint32_t blockSize, uint32_t blockSignature, FILE* fp) {
    WinShellink::ShellinkErr err;
    if (blockSize != extraData_.darDB.BlockSize)
        return (ShellinkErr::SHLLINK_ERRX_WRONGSIZE);

    //DarwinDataAnsi
    if ((err = readStr(extraData_.darDB.DarwinDataAnsi, ShellinkErr::SHLLINK_ERRX_NULLPSTRDARDA, ShellinkErr::SHLLINK_ERR_FIO, fp, 260)) != ShellinkErr::SHLLINK_ERR_NONE)
        return err;

    //DarwinDataUnicode
    if ((err = readWStr(extraData_.darDB.DarwinDataUnicode, ShellinkErr::SHLLINK_ERRX_NULLPSTRDARDU, ShellinkErr::SHLLINK_ERR_FIO, fp, 520)) != ShellinkErr::SHLLINK_ERR_NONE)
        return err;

    return ShellinkErr::SHLLINK_ERR_NONE;
}

WinShellink::ShellinkErr WinShellink::readEEnvironmentVariableDataBlock(uint32_t blockSize, uint32_t blockSignature, FILE* fp) {
    WinShellink::ShellinkErr err;
    if (blockSize != extraData_.envVarDB.BlockSize)
        return (ShellinkErr::SHLLINK_ERRX_WRONGSIZE);

    //EnvironmentVariableDataAnsi
    if ((err = readStr(extraData_.envVarDB.TargetAnsi, ShellinkErr::SHLLINK_ERRX_NULLPSTRENVDA, ShellinkErr::SHLLINK_ERR_FIO, fp, 260)) != ShellinkErr::SHLLINK_ERR_NONE)
        return err;

    //EnvironmentVariableDataUnicode
    if ((err = readWStr(extraData_.envVarDB.TargetUnicode, ShellinkErr::SHLLINK_ERRX_NULLPSTRENVDU, ShellinkErr::SHLLINK_ERR_FIO, fp, 520)) != ShellinkErr::SHLLINK_ERR_NONE)
        return err;

    return ShellinkErr::SHLLINK_ERR_NONE;
}

WinShellink::ShellinkErr WinShellink::readEIconEnvironmentDataBlock(uint32_t blockSize, uint32_t blockSignature, FILE* fp) {
    WinShellink::ShellinkErr err;
    if (blockSize != extraData_.iconEnvDB.BlockSize)
        return (ShellinkErr::SHLLINK_ERRX_WRONGSIZE);

    //IconEnvironmentDataAnsi
    if ((err = readStr(extraData_.iconEnvDB.TargetAnsi, ShellinkErr::SHLLINK_ERRX_NULLPSTRIENVDA, ShellinkErr::SHLLINK_ERR_FIO, fp, 260)) != ShellinkErr::SHLLINK_ERR_NONE)
        return err;

    //IconEnvironmentDataUnicode
    if ((err = readWStr(extraData_.iconEnvDB.TargetUnicode, ShellinkErr::SHLLINK_ERRX_NULLPSTRIENVDU, ShellinkErr::SHLLINK_ERR_FIO, fp, 520)) != ShellinkErr::SHLLINK_ERR_NONE)
        return err;

    return ShellinkErr::SHLLINK_ERR_NONE;
}

WinShellink::ShellinkErr WinShellink::readEKnownFolderDataBlock(uint32_t blockSize, uint32_t blockSignature, FILE* fp) {
    WinShellink::ShellinkErr err;
    if (blockSize != extraData_.knownFolderDB.BlockSize)
        return (ShellinkErr::SHLLINK_ERRX_WRONGSIZE);

    //KnownFolderID
    if ((err = readStr(extraData_.knownFolderDB.KnownFolderID, ShellinkErr::SHLLINK_ERR_NULLPEXTD, ShellinkErr::SHLLINK_ERR_FIO, fp, 16)) != ShellinkErr::SHLLINK_ERR_NONE)
        return err;

    //Offset
    if (fread(&extraData_.knownFolderDB.Offset, 4, 1, fp) != 1)
        return (ShellinkErr::SHLLINK_ERR_FIO);

    return ShellinkErr::SHLLINK_ERR_NONE;
}

WinShellink::ShellinkErr WinShellink::readEPropertyStoreDataBlock(uint32_t blockSize, uint32_t blockSignature, FILE* fp) {
    WinShellink::ShellinkErr err;
    if (blockSize < 0x0000000C)
        return (ShellinkErr::SHLLINK_ERRX_WRONGSIZE);
    extraData_.propStoreDB.BlockSize = blockSize;

    //PropertyStore
    if ((err = readStr(extraData_.propStoreDB.PropertyStore, ShellinkErr::SHLLINK_ERR_NULLPEXTD, ShellinkErr::SHLLINK_ERR_FIO, fp, extraData_.propStoreDB.BlockSize - 8)) != ShellinkErr::SHLLINK_ERR_NONE)
        return err;

    return ShellinkErr::SHLLINK_ERR_NONE;
}

WinShellink::ShellinkErr WinShellink::readEShimDataBlock(uint32_t blockSize, uint32_t blockSignature, FILE* fp) {
    WinShellink::ShellinkErr err;
    if (blockSize < 0x00000088)
        return (ShellinkErr::SHLLINK_ERRX_WRONGSIZE);
    extraData_.shimDB.BlockSize = blockSize;

    //LayerName
    if ((err = readWStr(extraData_.shimDB.LayerName, ShellinkErr::SHLLINK_ERR_NULLPEXTD, ShellinkErr::SHLLINK_ERR_FIO, fp, extraData_.shimDB.BlockSize - 8)) != ShellinkErr::SHLLINK_ERR_NONE)
        return err;

    return ShellinkErr::SHLLINK_ERR_NONE;
}

WinShellink::ShellinkErr WinShellink::readESpecialFolderDataBlock(uint32_t blockSize, uint32_t blockSignature, FILE* fp) {
    if (blockSize != extraData_.speFolderDB.BlockSize)
        return (ShellinkErr::SHLLINK_ERRX_WRONGSIZE);

    //SpecialFolderID
    if (fread(&extraData_.speFolderDB.SpecialFolderID, 4, 1, fp) != 1)
        return (ShellinkErr::SHLLINK_ERR_FIO);

    //Offset
    if (fread(&extraData_.speFolderDB.Offset, 4, 1, fp) != 1)
        return (ShellinkErr::SHLLINK_ERR_FIO);

    return ShellinkErr::SHLLINK_ERR_NONE;
}

WinShellink::ShellinkErr WinShellink::readETrackerDataBlock(uint32_t blockSize, uint32_t blockSignature, FILE* fp) {
    WinShellink::ShellinkErr err;
    if (blockSize != extraData_.trackerDB.BlockSize)
        return (ShellinkErr::SHLLINK_ERRX_WRONGSIZE);

    //Length
    if (fread(&extraData_.trackerDB.Length, 4, 1, fp) != 1)
        return (ShellinkErr::SHLLINK_ERR_FIO);
    if (extraData_.trackerDB.Length != 0x00000058)
        return (ShellinkErr::SHLLINK_ERRX_WRONGSIZE);

    //Version
    if (fread(&extraData_.trackerDB.Version, 4, 1, fp) != 1)
        return (ShellinkErr::SHLLINK_ERR_FIO);
    if (extraData_.trackerDB.Version != 0)
        return (ShellinkErr::SHLLINK_ERRX_WRONGVERSION);

    //MachineID
    if ((err = readStr(extraData_.trackerDB.MachineID, ShellinkErr::SHLLINK_ERR_NULLPEXTD, ShellinkErr::SHLLINK_ERR_FIO, fp, 16)) != ShellinkErr::SHLLINK_ERR_NONE)
        return err;

    //Droid
    if ((err = readStr(extraData_.trackerDB.Droid, ShellinkErr::SHLLINK_ERR_NULLPEXTD, ShellinkErr::SHLLINK_ERR_FIO, fp, 32)) != ShellinkErr::SHLLINK_ERR_NONE)
        return err;

    //DroidBirth
    if ((err = readStr(extraData_.trackerDB.DroidBirth, ShellinkErr::SHLLINK_ERR_NULLPEXTD, ShellinkErr::SHLLINK_ERR_FIO, fp, 32)) != ShellinkErr::SHLLINK_ERR_NONE)
        return err;

    return ShellinkErr::SHLLINK_ERR_NONE;
}

WinShellink::ShellinkErr WinShellink::readEVistaAndAboveIDListDataBlock(uint32_t blockSize, uint32_t blockSignature, FILE* fp) {
    if (blockSize < 0x0000000A)
        return (ShellinkErr::SHLLINK_ERRX_WRONGSIZE);
    extraData_.vistaAboveIDListDB.BlockSize = blockSize;

    if (fread(&extraData_.vistaAboveIDListDB.targetIdList.idListSize, 2, 1, fp) != 1)
        return (ShellinkErr::SHLLINK_ERR_FIO);

    extraData_.vistaAboveIDListDB.targetIdList.IDListData.resize(extraData_.vistaAboveIDListDB.targetIdList.idListSize);
    if (fread(&extraData_.vistaAboveIDListDB.targetIdList.IDListData[0], extraData_.vistaAboveIDListDB.targetIdList.idListSize, 1, fp) != 1)
        return (ShellinkErr::SHLLINK_ERR_FIO);

#if 0
    int tmpS = blockSize - 10;
    while (tmpS > 0) {
        uint16_t itemSize = 0;
        //Element size
        if (fread(&itemSize, 2, 1, fp) != 1)
            return (ShellinkErr::SHLLINK_ERR_FIO);

        //Element (size -2 as struct contains one 2byte var)
        uint16_t itemDataSize = itemSize - 2;
        std::vector<uint8_t> itemData((size_t)(itemDataSize));

        if (itemData.size() != itemDataSize)
            return (ShellinkErr::SHLLINK_ERR_NULLPIDLM);

        if (fread(&itemData[0], itemDataSize, 1, fp) != 1)
            return (ShellinkErr::SHLLINK_ERR_FIO);

        tmpS -= itemSize;
        extraData_.vistaAboveIDListDB.targetIdList.itemIDList.push_back(itemData);
    }

    uint16_t nullb;
    if (fread(&nullb, 2, 1, fp) != 1)
        return (ShellinkErr::SHLLINK_ERR_FIO);

    if (tmpS < 0 || nullb != 0)
        return (ShellinkErr::SHLLINK_ERR_INVIDL);
#endif
    return ShellinkErr::SHLLINK_ERR_NONE;
}

bool WinShellink::saveAs(const std::wstring& lnkPath) {
    return true;
}

WinShellink::ShellinkErr WinShellink::readNULLStr(char** dest, WinShellink::ShellinkErr errv1, WinShellink::ShellinkErr errv2, FILE* fp) {
    char tmpC = 1;
    uint32_t tmpS = 0;
    while (tmpC != 0) {
        *dest = (char*)realloc(*dest, tmpS + 1);
        if (*dest == NULL)
            return (errv1);
        if (fread(&(*dest)[tmpS], 1, 1, fp) != 1)
            return (errv2);
        tmpC = (*dest)[tmpS];
        tmpS++;
    }
    return ShellinkErr::SHLLINK_ERR_NONE;
}

WinShellink::ShellinkErr WinShellink::readNULLWStr(wchar_t** dest, WinShellink::ShellinkErr errv1, WinShellink::ShellinkErr errv2, FILE* fp) {
    char16_t tmpC = 1;
    uint32_t tmpS = 0;
    while (tmpC != 0) {
        *dest = (wchar_t*)realloc(*dest, (tmpS + 1) * sizeof(wchar_t));
        if (*dest == NULL)
            return (errv1);
        if (fread(&(*dest)[tmpS], sizeof(wchar_t), 1, fp) != 1)
            return (errv2);
        tmpC = (*dest)[tmpS];
        tmpS++;
    }
    return ShellinkErr::SHLLINK_ERR_NONE;
}

WinShellink::ShellinkErr WinShellink::readNULLStr(std::vector<char>& dest, WinShellink::ShellinkErr errv1, WinShellink::ShellinkErr errv2, FILE* fp) {
    char tmpC = 1;
    uint32_t tmpS = 0;
    while (tmpC != 0) {
        if (dest.size() != (tmpS + 1))
            dest.resize(tmpS + 1);
        if (dest.size() != (tmpS + 1))
            return errv1;

        if (fread(&dest[tmpS], 1, 1, fp) != 1)
            return errv2;
        tmpC = dest[tmpS];
        tmpS++;
    }
    return ShellinkErr::SHLLINK_ERR_NONE;
}

WinShellink::ShellinkErr WinShellink::readNULLWStr(std::vector<wchar_t>& dest, WinShellink::ShellinkErr errv1, WinShellink::ShellinkErr errv2, FILE* fp) {
    char16_t tmpC = 1;
    uint32_t tmpS = 0;
    while (tmpC != 0) {
        if (dest.size() != (tmpS + 1))
            dest.resize(tmpS + 1);

        if (dest.size() != (tmpS + 1))
            return errv1;

        if (fread(&dest[tmpS], sizeof(wchar_t), 1, fp) != 1)
            return errv2;
        tmpC = dest[tmpS];
        tmpS++;
    }
    return ShellinkErr::SHLLINK_ERR_NONE;
}

WinShellink::ShellinkErr WinShellink::readStr(std::string& dest, WinShellink::ShellinkErr errv1, WinShellink::ShellinkErr errv2, FILE* fp, size_t size) {
    if (size == 0)
        return (errv1);
    char* buf = (char*)malloc(size + 1);
    if (buf == NULL)
        return (errv1);
    memset(buf, 0, size + 1);
    if (fread(buf, 1, size, fp) != size) {
        free(buf);
        return (errv2);
    }

    dest = buf;
    free(buf);

    return ShellinkErr::SHLLINK_ERR_NONE;
}

WinShellink::ShellinkErr WinShellink::readWStr(std::wstring& dest, WinShellink::ShellinkErr errv1, WinShellink::ShellinkErr errv2, FILE* fp, size_t size) {
    size /= 2;
    if (size == 0)
        return (errv1);
    wchar_t* buf = (wchar_t*)malloc((size + 1) * sizeof(wchar_t));
    if (buf == NULL)
        return (errv1);
    memset(buf, 0, (size + 1) * sizeof(wchar_t));
    if (fread(buf, sizeof(wchar_t), size, fp) != size) {
        free(buf);
        return (errv2);
    }

    dest = buf;
    free(buf);

    return ShellinkErr::SHLLINK_ERR_NONE;
}

void WinShellink::toBigEndian(void* inp, size_t size) {
    for (int i = 0; i < size / 2; i++) {
        uint8_t t = ((uint8_t*)inp)[size - 1 - i];
        ((uint8_t*)inp)[size - 1 - i] = ((uint8_t*)inp)[i];
        ((uint8_t*)inp)[i] = t;
    }
}

bool WinShellink::IsResourceString(const std::wstring& s) {
    std::wstring s2 = StringHelper::Trim(s, L" \"");
    return StringHelper::IsStartsWith(s2, L"@");
}

bool WinShellink::LoadStringFromRes(const std::wstring& resStr, std::wstring& result) {
    std::wstring resStrFormat = StringHelper::Trim(resStr, L" \"");
    resStrFormat = resStrFormat.substr(1);  // @

    if (resStrFormat.empty())
        return false;

    std::vector<std::wstring> v = StringHelper::Split(resStrFormat, L",", true);
    if (v.size() < 2)
        return false;

    if (v[0].empty() || v[1].empty())
        return false;

    std::wstring envExpanded = PathUtil::ExpandEnvString(v[0]);
    HMODULE hDll = LoadLibraryW(envExpanded.c_str());
    if (!hDll) {
        DWORD dwGLE = GetLastError();
        return false;
    }

    if (StringHelper::IsStartsWith(v[1], L"-"))
        v[1] = v[1].substr(1);

    UINT id = _wtoi(v[1].c_str());

    wchar_t szBuf[MAX_PATH + 1] = {0};
    int numberOfBytes = LoadStringW(hDll, id, szBuf, MAX_PATH);
    if (numberOfBytes <= 0) {
        FreeLibrary(hDll);
        return false;
    }

    result = szBuf;
    FreeLibrary(hDll);

    return true;
}

std::wstring WinShellink::getDisplayName() const {
    std::wstring result;
    if (header_.LinkFlags & ShllinkLinkFlag::LF_HasName) {
        if (header_.LinkFlags & ShllinkLinkFlag::LF_IsUnicode)
            result = stringData_.NameStringW;
        else
            result = StringEncode::AnsiToUnicode(stringData_.NameStringA);
    }

    return result;
}

std::wstring WinShellink::getTargetPath() const {
    std::wstring result;
    if ((header_.LinkFlags & ShllinkLinkFlag::LF_HasLinkInfo) && !(header_.LinkFlags & ShllinkLinkFlag::LF_ForceNoLinkInfo)) {
        if (linkInfo_.LnkInfFlags & LinkInfoFlag::LIF_VolumeIDAndLocalBasePath) {
            if (linkInfo_.LinkInfoHeaderSize >= 0x00000024)
                result = linkInfo_.LocalBasePathUnicode + linkInfo_.CommonPathSuffixUnicode;
            else
                result = StringEncode::AnsiToUnicode(linkInfo_.LocalBasePath + linkInfo_.CommonPathSuffix);
        }
        else if (linkInfo_.LnkInfFlags & LinkInfoFlag::LIF_CommonNetworkRelativeLinkAndPathSuffix) {
            if (linkInfo_.CommonNetRelLnk.ComNetRelLnkFlags & CommonNetworkRelativeLinkFlag::CNETRLNK_ValidDevice) {
                if (linkInfo_.CommonNetRelLnk.NetNameOffset > 0x00000014)
                    result = linkInfo_.CommonNetRelLnk.DeviceNameUnicode;
                else
                    result = StringEncode::AnsiToUnicode(linkInfo_.CommonNetRelLnk.DeviceName);
            }
            else if (linkInfo_.CommonNetRelLnk.ComNetRelLnkFlags & CommonNetworkRelativeLinkFlag::CNETRLNK_ValidNetType) {
                if (linkInfo_.CommonNetRelLnk.NetNameOffset > 0x00000014)
                    result = linkInfo_.CommonNetRelLnk.NetNameUnicode;
                else
                    result = StringEncode::AnsiToUnicode(linkInfo_.CommonNetRelLnk.NetName);
            }
        }
    }
    if (!result.empty())
        return result;

    if (extraData_.speFolderDB.SpecialFolderID != 0) {
        wchar_t szPath[MAX_PATH + 1] = {0};
        if (S_OK == SHGetFolderPathW(NULL, extraData_.speFolderDB.SpecialFolderID, NULL, SHGFP_TYPE_CURRENT, szPath)) {
            result = szPath;
        }
    }
    if (!result.empty())
        return result;

    if (header_.LinkFlags & ShllinkLinkFlag::LF_HasLinkTargetIDList) {
        ITEMIDLIST* pIDL = (ITEMIDLIST*)(&targetIdList_.IDListData[0]);
        wchar_t szPath[MAX_PATH + 1] = {0};
        if (SHGetPathFromIDListW(pIDL, szPath)) {
            result = szPath;
        }
    }
    if (!result.empty())
        return result;

    if (header_.LinkFlags & ShllinkLinkFlag::LF_HasExpString) {
        if (!extraData_.envVarDB.TargetUnicode.empty())
            result = extraData_.envVarDB.TargetUnicode;
        else if (!extraData_.envVarDB.TargetAnsi.empty())
            result = StringEncode::AnsiToUnicode(extraData_.envVarDB.TargetAnsi);
    }

    return result;
}

std::wstring WinShellink::getArguments() const {
    std::wstring result;
    if (header_.LinkFlags & ShllinkLinkFlag::LF_HasArguments) {
        if (header_.LinkFlags & ShllinkLinkFlag::LF_IsUnicode)
            result = stringData_.CommandLineArgumentsW;
        else
            result = StringEncode::AnsiToUnicode(stringData_.CommandLineArgumentsA);
    }
    return result;
}

std::wstring WinShellink::getIconPath() const {
    std::wstring result;
    if (header_.LinkFlags & ShllinkLinkFlag::LF_HasExpIcon) {
        if (!extraData_.iconEnvDB.TargetUnicode.empty())
            result = extraData_.iconEnvDB.TargetUnicode;
        else if (!extraData_.iconEnvDB.TargetAnsi.empty())
            result = StringEncode::AnsiToUnicode(extraData_.iconEnvDB.TargetAnsi);
    }

    if (!result.empty())
        return result;

    if (header_.LinkFlags & ShllinkLinkFlag::LF_HasIconLocation) {
        if (header_.LinkFlags & ShllinkLinkFlag::LF_IsUnicode)
            result = stringData_.IconLocationW;
        else
            result = StringEncode::AnsiToUnicode(stringData_.IconLocationA);
    }

    return result;
}

int32_t WinShellink::getIconIndex() const {
    return header_.IconIndex;
}

}  // namespace jhc
#endif