#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <exception>
#include "json.hpp"

using json = nlohmann::json;


constexpr uint32_t HASH_UObjectProperty = 0xEAB3;
constexpr uint32_t HASH_UStrProperty = 0x2472;
constexpr uint32_t HASH_UByteProperty = 0xC02D;
constexpr uint32_t HASH_UIntProperty = 0x4A36;
constexpr uint32_t HASH_UNameProperty = 0x4A08;
constexpr uint32_t HASH_UArrayProperty = 0x69E3;
constexpr uint32_t HASH_UBoolProperty = 0x8AB0;
constexpr uint32_t HASH_UUInt32Property = 0x4A38;
constexpr uint32_t HASH_UFloatProperty = 0xFDDE;
constexpr uint32_t HASH_UTextProperty = 0xB774;
constexpr uint32_t HASH_USoftObjectProperty = 0xFAAE;
constexpr uint32_t HASH_UEnumProperty = 0x409D;
constexpr uint32_t HASH_UStructProperty = 0xFC9C;

// Structures for storing various data

struct ThumbnailIndex {
	std::string AssetClassName;
	std::string ObjectPathWithoutPackageName;
	int32_t FileOffset;
};

struct Thumbnail {
	int32_t ImageWidth;
	int32_t ImageHeight;
	std::string ImageFormat;
	int32_t ImageSizeData;
	std::vector<uint8_t> ImageData;
};

struct Tag {
	std::string Key;
	std::string Value;
};

struct AssetRegistryEntry {
	std::string ObjectPath;
	std::string ObjectClassName;
	std::vector<Tag> Tags;
};

struct AssetRegistryData {
	int64_t DependencyDataOffset;
	int32_t size;
	std::vector<AssetRegistryEntry> data;
};

struct UassetData {
	struct Header {
		uint32_t EPackageFileTag;
		int32_t LegacyFileVersion;
		int32_t LegacyUE3Version;
		int32_t FileVersionUE4;
		int32_t FileVersionUE5;
		int32_t FileVersionLicenseeUE4;
		std::vector<std::pair<std::string, int32_t>> CustomVersions;
		int32_t TotalHeaderSize;
		std::string FolderName;
		uint32_t PackageFlags;
		int32_t NameCount;
		int32_t NameOffset;
		uint32_t SoftObjectPathsCount;
		uint32_t SoftObjectPathsOffset;
		std::string LocalizationId;
		int32_t GatherableTextDataCount;
		int32_t GatherableTextDataOffset;
		int32_t ExportCount;
		int32_t ExportOffset;
		int32_t ImportCount;
		int32_t ImportOffset;
		int32_t DependsOffset;
		uint32_t SoftPackageReferencesCount;
		uint32_t SoftPackageReferencesOffset;
		int32_t SearchableNamesOffset;
		int32_t ThumbnailTableOffset;
		std::string Guid;
		std::string PersistentGuid;
		std::string OwnerPersistentGuid;
		std::vector<std::pair<int32_t, int32_t>> Generations;
		std::string SavedByEngineVersion;
		std::string CompatibleWithEngineVersion;
		uint32_t CompressionFlags;
		uint32_t PackageSource;
		uint32_t AdditionalPackagesToCookCount;
		int32_t NumTextureAllocations;
		int32_t AssetRegistryDataOffset;
		int64_t BulkDataStartOffset;
		int32_t WorldTileInfoDataOffset;
		std::vector<int32_t> ChunkIDs;
		int32_t ChunkID;
		int32_t PreloadDependencyCount;
		int32_t PreloadDependencyOffset;
		int32_t NamesReferencedFromExportDataCount;
		int64_t PayloadTocOffset;
		int32_t DataResourceOffset;
		int32_t EngineChangelist;
	} header;

	struct Import {
		std::string classPackage;
		std::string className;
		int32_t outerIndex;
		std::string objectName;
		std::string packageName;
		int32_t bImportOptional;
	};

	struct Export {
		int32_t classIndex;
		int32_t superIndex;
		int32_t templateIndex;
		int32_t outerIndex;
		std::string objectName;
		uint32_t objectFlags;
		int64_t serialSize;
		int64_t serialOffset;
		int32_t bForcedExport;
		int32_t bNotForClient;
		int32_t bNotForServer;
		std::string packageGuid;
		uint32_t packageFlags;
		int32_t bNotAlwaysLoadedForEditorGame;
		int32_t bIsAsset;
		int32_t bGeneratePublicHash;
		int32_t firstExportDependency;
		int32_t serializationBeforeSerializationDependencies;
		int32_t createBeforeSerializationDependencies;
		int32_t serializationBeforeCreateDependencies;
		int32_t createBeforeCreateDependencies;
		std::vector<std::string> data;
		std::vector<uint8_t> chunkData;

		struct ObjectMetadata {
			std::string ObjectClass;
			std::string ObjectName;
			std::string OuterObject;
		} metadata;

		struct Property {
			std::string PropertyName;
			std::string PropertyType;

			int intValue;
			float floatValue;
			bool boolValue;
			std::string stringValue;
			Property() : PropertyName(""), PropertyType("") {}
			~Property() {}

			// Copy constructor
			Property(const Property& other) : PropertyName(other.PropertyName), PropertyType(other.PropertyType) {
				if (PropertyType == "int") {
					intValue = other.intValue;
				}
				else if (PropertyType == "float") {
					floatValue = other.floatValue;
				}
				else if (PropertyType == "bool") {
					boolValue = other.boolValue;
				}
				else if (PropertyType == "FString") {
					new (&stringValue) std::string(other.stringValue);
				}
			}

			// Copy assignment operator
			Property& operator=(const Property& other) {
				if (this == &other) {
					return *this;
				}

				PropertyName = other.PropertyName;
				PropertyType = other.PropertyType;

				if (PropertyType == "int") {
					intValue = other.intValue;
				}
				else if (PropertyType == "float") {
					floatValue = other.floatValue;
				}
				else if (PropertyType == "bool") {
					boolValue = other.boolValue;
				}
				else if (PropertyType == "FString") {
					stringValue = other.stringValue;
				}

				return *this;
			}
		};

		std::vector<Property> properties;
		int internalIndex;

	};

	struct Name {
		std::string Name;
		uint16_t NonCasePreservingHash;
		uint16_t CasePreservingHash;
	};

	struct GatherableTextData {
		std::string NamespaceName;
		struct SourceDataStruct {
			std::string SourceString;
			struct SourceStringMetaDataStruct {
				int32_t ValueCount;
				std::vector<std::string> Values;
			} SourceStringMetaData;
		} SourceData;
		struct SourceSiteContextStruct {
			std::string KeyName;
			std::string SiteDescription;
			uint32_t IsEditorOnly;
			uint32_t IsOptional;
			struct InfoMetaDataStruct {
				int32_t ValueCount;
				std::vector<std::string> Values;
			} InfoMetaData;
			struct KeyMetaDataStruct {
				int32_t ValueCount;
				std::vector<std::string> Values;
			} KeyMetaData;
		};
		std::vector<SourceSiteContextStruct> SourceSiteContexts;
	};

	std::vector<Name> names;
	std::vector<Import> imports;
	std::vector<Export> exports;
	std::vector<GatherableTextData> gatherableTextData;
	std::vector<ThumbnailIndex> thumbnailsIndex;
	std::vector<Thumbnail> thumbnails;
	AssetRegistryData assetRegistryData;
};

// Custom exception class for handling parsing errors
class ParseException : public std::exception {
public:
	explicit ParseException(const std::string& message) : msg_(message) {}
	virtual const char* what() const noexcept { return msg_.c_str(); }
private:
	std::string msg_;
};

// Function to convert a GUID to a string
std::string guidToString(uint8_t guid[16]) {
	std::ostringstream ss;
	for (int i = 0; i < 16; ++i) {
		if (i == 4 || i == 6 || i == 8 || i == 10)
			ss << '-';
		ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(guid[i]);
	}
	return ss.str();
}

#define REFLECTABLE_CLASS  \
public: \
    static const char* GetClassName() { return __FUNCTION__; }



class Uasset {
public:
	UassetData data;
	REFLECTABLE_CLASS
		bool parse(const std::vector<uint8_t>& bytes);
	json toJson() const;
private:
	size_t currentIdx = 0;
	const std::vector<uint8_t>* bytesPtr = nullptr;

	uint16_t readUint16();
	int32_t readInt32();
	uint32_t readUint32();
	int64_t readInt64();
	int64_t readInt64Export();
	//    uint64_t readUint64();
	std::string readFString();
	std::string readGuid();
	std::vector<uint8_t> readCountBytes(int64_t count);
	float readFloat();
	bool readBool();

	void readAssetRegistryData();
	bool readHeader();
	void readNames();
	bool readGatherableTextData();
	void readImports();
	void readExports();
	void readExportData(UassetData::Export& exportData);
	std::string determineStructureType(const std::string& objectClass);
	void processParentClass(UassetData::Export& exportData, size_t& exportDataIdx);
	void processDynamicBindingObjects(UassetData::Export& exportData, size_t& exportDataIdx);
	void processUberGraphFrame(UassetData::Export& exportData, size_t& exportDataIdx);
	void processSchema(UassetData::Export& exportData, size_t& exportDataIdx);
	void processbCommentBubbleVisible_InDetailsPanel(UassetData::Export& exportData, size_t& exportDataIdx);
	void processbCommentBubbleVisible(UassetData::Export& exportData, size_t& exportDataIdx);
	void processbCommentBubblePinned(UassetData::Export& exportData, size_t& exportDataIdx);
	void processNone(UassetData::Export& exportData, size_t& exportDataIdx);
	void processInputKeyDelegateBindings(UassetData::Export& exportData, size_t& exportDataIdx);
	void processDelegateReference(UassetData::Export& exportData, size_t& exportDataIdx);
	void processMemberReference(UassetData::Export& exportData, size_t& exportDataIdx);
	void processMemberParent(UassetData::Export& exportData, size_t& exportDataIdx);
	void processMemberName(UassetData::Export& exportData, size_t& exportDataIdx);
	void processBlueprintSystemVersion(UassetData::Export& exportData, size_t& exportDataIdx);
	void processSimpleConstructionScript(UassetData::Export& exportData, size_t& exportDataIdx);
	void processUbergraphPages(UassetData::Export& exportData, size_t& exportDataIdx);
	void processUberGraphFunction(UassetData::Export& exportData, size_t& exportDataIdx);
	void processPackage(UassetData::Export& exportData, size_t& exportDataIdx);
	void processFunctionGraphs(UassetData::Export& exportData, size_t& exportDataIdx);
	void processFunctionReference(UassetData::Export& exportData, size_t& exportDataIdx);
	void processbIsPureFunc(UassetData::Export& exportData, size_t& exportDataIdx);
	void processbIsConstFunc(UassetData::Export& exportData, size_t& exportDataIdx);
	void processNodePosX(UassetData::Export& exportData, size_t& exportDataIdx);
	void processNodePosY(UassetData::Export& exportData, size_t& exportDataIdx);
	void processNodeWidth(UassetData::Export& exportData, size_t& exportDataIdx);
	void processNodeHeight(UassetData::Export& exportData, size_t& exportDataIdx);
	void processNodeComment(UassetData::Export& exportData, size_t& exportDataIdx);
	void processCustomFunctionName(UassetData::Export& exportData, size_t& exportDataIdx);
	void processEventReference(UassetData::Export& exportData, size_t& exportDataIdx);
	void processExtraFlags(UassetData::Export& exportData, size_t& exportDataIdx);
	void processCustomClass(UassetData::Export& exportData, size_t& exportDataIdx);
	void processInputKey(UassetData::Export& exportData, size_t& exportDataIdx);
	void processVariableReference(UassetData::Export& exportData, size_t& exportDataIdx);
	void processbVisualizeComponent(UassetData::Export& exportData, size_t& exportDataIdx);
	void processComponentClass(UassetData::Export& exportData, size_t& exportDataIdx);
	void processRootNodes(UassetData::Export& exportData, size_t& exportDataIdx);
	void processNodes(UassetData::Export& exportData, size_t& exportDataIdx);
	void processGraphGuid(UassetData::Export& exportData, size_t& exportDataIdx);
	void processNodeGuid(UassetData::Export& exportData, size_t& exportDataIdx);
	void processMemberGuid(UassetData::Export& exportData, size_t& exportDataIdx);
	void processDefault(UassetData::Export& exportData, size_t& exportDataIdx);
	void processbAllowDeletion(UassetData::Export& exportData, size_t& exportDataIdx);
	uint8_t readByte();

	void readThumbnails();
	std::string resolveFName(int64_t idx);
};

uint8_t Uasset::readByte() {
	if (currentIdx + sizeof(uint8_t) > bytesPtr->size()) {
		throw ParseException("Out of bounds read (byte)");
	}
	uint8_t val = (*bytesPtr)[currentIdx];
	currentIdx += sizeof(val);
	return val;
}

bool Uasset::parse(const std::vector<uint8_t>& bytes) {
	const char* t = Uasset::GetClassName();
	currentIdx = 0;
	bytesPtr = &bytes;

	try {
		if (!readHeader()) {
			throw ParseException("Failed to read header");
		}

		readNames();

		if (!readGatherableTextData()) {
			throw ParseException("Failed to read gatherable text data");
		}

		readImports();
		readExports();
		readThumbnails();
		//       readAssetRegistryData();
		return true;
	}
	catch (const ParseException& e) {
		std::cerr << e.what() << std::endl;
		return false;
	}
}

bool Uasset::readHeader() {
	data.header.EPackageFileTag = readUint32();
	std::cout << "EPackageFileTag: " << data.header.EPackageFileTag << std::endl;

	data.header.LegacyFileVersion = readInt32();
	std::cout << "LegacyFileVersion: " << data.header.LegacyFileVersion << std::endl;

	data.header.LegacyUE3Version = readInt32();
	std::cout << "LegacyUE3Version: " << data.header.LegacyUE3Version << std::endl;

	data.header.FileVersionUE4 = readInt32();
	std::cout << "FileVersionUE4: " << data.header.FileVersionUE4 << std::endl;

	if (data.header.LegacyFileVersion <= -8) {
		data.header.FileVersionUE5 = readInt32();
		std::cout << "FileVersionUE5: " << data.header.FileVersionUE5 << std::endl;
	}

	data.header.FileVersionLicenseeUE4 = readInt32();
	std::cout << "FileVersionLicenseeUE4: " << data.header.FileVersionLicenseeUE4 << std::endl;

	int32_t customVersionsCount = readInt32();
	std::cout << "CustomVersions Count: " << customVersionsCount << std::endl;
	for (int32_t i = 0; i < customVersionsCount; ++i) {
		std::string key = readGuid();
		int32_t version = readInt32();
		data.header.CustomVersions.push_back({ key, version });
		std::cout << "CustomVersion[" << i << "]: " << key << " - " << version << std::endl;
	}

	data.header.TotalHeaderSize = readInt32();
	std::cout << "TotalHeaderSize: " << data.header.TotalHeaderSize << std::endl;

	data.header.FolderName = readFString();
	std::cout << "FolderName: " << data.header.FolderName << std::endl;

	data.header.PackageFlags = readUint32();
	std::cout << "PackageFlags: " << data.header.PackageFlags << std::endl;

	data.header.NameCount = readInt32();
	std::cout << "NameCount: " << data.header.NameCount << std::endl;

	data.header.NameOffset = readInt32();
	std::cout << "NameOffset: " << data.header.NameOffset << std::endl;

	if (data.header.FileVersionUE5 >= 0x0151) { // VER_UE5_ADD_SOFTOBJECTPATH_LIST
		data.header.SoftObjectPathsCount = readUint32();
		std::cout << "SoftObjectPathsCount: " << data.header.SoftObjectPathsCount << std::endl;
		data.header.SoftObjectPathsOffset = readUint32();
		std::cout << "SoftObjectPathsOffset: " << data.header.SoftObjectPathsOffset << std::endl;
	}

	data.header.LocalizationId = readFString();
	std::cout << "LocalizationId: " << data.header.LocalizationId << std::endl;

	data.header.GatherableTextDataCount = readInt32();
	std::cout << "GatherableTextDataCount: " << data.header.GatherableTextDataCount << std::endl;
	data.header.GatherableTextDataOffset = readInt32();
	std::cout << "GatherableTextDataOffset: " << data.header.GatherableTextDataOffset << std::endl;

	data.header.ExportCount = readInt32();
	std::cout << "ExportCount: " << data.header.ExportCount << std::endl;
	data.header.ExportOffset = readInt32();
	std::cout << "ExportOffset: " << data.header.ExportOffset << std::endl;
	data.header.ImportCount = readInt32();
	std::cout << "ImportCount: " << data.header.ImportCount << std::endl;
	data.header.ImportOffset = readInt32();
	std::cout << "ImportOffset: " << data.header.ImportOffset << std::endl;
	data.header.DependsOffset = readInt32();
	std::cout << "DependsOffset: " << data.header.DependsOffset << std::endl;

	if (data.header.FileVersionUE4 >= 0x0154) { // VER_UE4_ADD_STRING_ASSET_REFERENCES_MAP
		data.header.SoftPackageReferencesCount = readInt32();
		std::cout << "SoftPackageReferencesCount: " << data.header.SoftPackageReferencesCount << std::endl;
		data.header.SoftPackageReferencesOffset = readInt32();
		std::cout << "SoftPackageReferencesOffset: " << data.header.SoftPackageReferencesOffset << std::endl;
	}

	if (data.header.FileVersionUE4 >= 0x0163) { // VER_UE4_ADDED_SEARCHABLE_NAMES
		data.header.SearchableNamesOffset = readInt32();
		std::cout << "SearchableNamesOffset: " << data.header.SearchableNamesOffset << std::endl;
	}

	data.header.ThumbnailTableOffset = readInt32();
	std::cout << "ThumbnailTableOffset: " << data.header.ThumbnailTableOffset << std::endl;
	data.header.Guid = readGuid();
	std::cout << "Guid: " << data.header.Guid << std::endl;

	if (data.header.FileVersionUE4 >= 0x0166) { // VER_UE4_ADDED_PACKAGE_OWNER
		data.header.PersistentGuid = readGuid();
		std::cout << "PersistentGuid: " << data.header.PersistentGuid << std::endl;
	}

	if (data.header.FileVersionUE4 >= 0x0166 && data.header.FileVersionUE4 < 0x0183) { // VER_UE4_NON_OUTER_PACKAGE_IMPORT
		data.header.OwnerPersistentGuid = readGuid();
		std::cout << "OwnerPersistentGuid: " << data.header.OwnerPersistentGuid << std::endl;
	}

	int32_t generationsCount = readInt32();
	data.header.Generations.clear();
	for (int32_t i = 0; i < generationsCount; ++i) {
		int32_t exportCount = readInt32();
		int32_t nameCount = readInt32();
		data.header.Generations.push_back({ exportCount, nameCount });
	}

	if (data.header.FileVersionUE4 >= 0x0171) { // VER_UE4_ENGINE_VERSION_OBJECT
		data.header.SavedByEngineVersion = std::to_string(readUint16()) + "." +
			std::to_string(readUint16()) + "." +
			std::to_string(readUint16()) + "-" +
			std::to_string(readUint32()) + "+" +
			readFString();
	}
	else {
		data.header.EngineChangelist = readInt32();
	}

	if (data.header.FileVersionUE4 >= 0x0175) { // VER_UE4_PACKAGE_SUMMARY_HAS_COMPATIBLE_ENGINE_VERSION
		data.header.CompatibleWithEngineVersion = std::to_string(readUint16()) + "." +
			std::to_string(readUint16()) + "." +
			std::to_string(readUint16()) + "-" +
			std::to_string(readUint32()) + "+" +
			readFString();
	}
	else {
		data.header.CompatibleWithEngineVersion = data.header.SavedByEngineVersion;
	}

	data.header.CompressionFlags = readUint32();

	int32_t compressedChunksCount = readInt32();
	if (compressedChunksCount > 0) {
		throw ParseException("Asset compressed");
	}

	data.header.PackageSource = readUint32();
	data.header.AdditionalPackagesToCookCount = readUint32();
	if (data.header.AdditionalPackagesToCookCount > 0) {
		throw ParseException("AdditionalPackagesToCook has items");
	}

	if (data.header.LegacyFileVersion > -7) {
		data.header.NumTextureAllocations = readInt32();
	}

	data.header.AssetRegistryDataOffset = readInt32();
	data.header.BulkDataStartOffset = readInt64();

	if (data.header.FileVersionUE4 >= 0x0183) { // VER_UE4_WORLD_LEVEL_INFO
		data.header.WorldTileInfoDataOffset = readInt32();
	}

	if (data.header.FileVersionUE4 >= 0x0191) { // VER_UE4_CHANGED_CHUNKID_TO_BE_AN_ARRAY_OF_CHUNKIDS
		int32_t chunkIDsCount = readInt32();
		data.header.ChunkIDs.clear();
		if (chunkIDsCount > 0) {
			for (int32_t i = 0; i < chunkIDsCount; ++i) {
				data.header.ChunkIDs.push_back(readInt32());
			}
		}
	}
	else if (data.header.FileVersionUE4 >= 0x0192) { // VER_UE4_ADDED_CHUNKID_TO_ASSETDATA_AND_UPACKAGE
		data.header.ChunkID = readInt32();
	}

	if (data.header.FileVersionUE4 >= 0x0194) { // VER_UE4_PRELOAD_DEPENDENCIES_IN_COOKED_EXPORTS
		data.header.PreloadDependencyCount = readInt32();
		data.header.PreloadDependencyOffset = readInt32();
	}
	else {
		data.header.PreloadDependencyCount = -1;
		data.header.PreloadDependencyOffset = 0;
	}

	if (data.header.FileVersionUE5 >= 0x0196) { // VER_UE5_NAMES_REFERENCED_FROM_EXPORT_DATA
		data.header.NamesReferencedFromExportDataCount = readInt32();
	}

	if (data.header.FileVersionUE5 >= 0x0197) { // VER_UE5_PAYLOAD_TOC
		data.header.PayloadTocOffset = readInt64();
	}
	else {
		data.header.PayloadTocOffset = -1;
	}

	if (data.header.FileVersionUE5 >= 0x0198) { // VER_UE5_DATA_RESOURCES
		data.header.DataResourceOffset = readInt32();
	}

	return true;
}

void Uasset::readNames() {
	currentIdx = data.header.NameOffset;
	data.names.clear();
	for (int32_t i = 0; i < data.header.NameCount; ++i) {
		UassetData::Name name;
		name.Name = readFString();
		name.NonCasePreservingHash = readUint16();
		name.CasePreservingHash = readUint16();
		data.names.push_back(name);
	}
}

bool Uasset::readGatherableTextData() {
	currentIdx = data.header.GatherableTextDataOffset;
	data.gatherableTextData.clear();
	for (int32_t i = 0; i < data.header.GatherableTextDataCount; ++i) {
		UassetData::GatherableTextData gatherableTextData;

		gatherableTextData.NamespaceName = readFString();
		gatherableTextData.SourceData.SourceString = readFString();
		gatherableTextData.SourceData.SourceStringMetaData.ValueCount = readInt32();

		if (gatherableTextData.SourceData.SourceStringMetaData.ValueCount > 0) {
			throw ParseException("Unsupported SourceStringMetaData from readGatherableTextData");
		}

		int32_t countSourceSiteContexts = readInt32();
		for (int32_t j = 0; j < countSourceSiteContexts; ++j) {
			UassetData::GatherableTextData::SourceSiteContextStruct sourceSiteContext;
			sourceSiteContext.KeyName = readFString();
			sourceSiteContext.SiteDescription = readFString();
			sourceSiteContext.IsEditorOnly = readUint32();
			sourceSiteContext.IsOptional = readUint32();

			sourceSiteContext.InfoMetaData.ValueCount = readInt32();
			if (sourceSiteContext.InfoMetaData.ValueCount > 0) {
				throw ParseException("Unsupported SourceSiteContexts.InfoMetaData from readGatherableTextData");
			}

			sourceSiteContext.KeyMetaData.ValueCount = readInt32();
			if (sourceSiteContext.KeyMetaData.ValueCount > 0) {
				throw ParseException("Unsupported SourceSiteContexts.KeyMetaData from readGatherableTextData");
			}

			gatherableTextData.SourceSiteContexts.push_back(sourceSiteContext);
		}

		data.gatherableTextData.push_back(gatherableTextData);
	}
	return true;
}

void Uasset::readImports() {
	currentIdx = data.header.ImportOffset;
	data.imports.clear();

	for (int32_t i = 0; i < data.header.ImportCount; ++i) {
		UassetData::Import importA;

		// Read indices and resolve names
		int64_t classPackageIdx = readInt64();
		int64_t classNameIdx = readInt64();
		importA.outerIndex = readInt32();
		int64_t objectNameIdx = readInt64();

		importA.classPackage = resolveFName(classPackageIdx);
		importA.className = resolveFName(classNameIdx);
		importA.objectName = resolveFName(objectNameIdx);

		if (data.header.FileVersionUE4 >= 0x0166) { // VER_UE4_NON_OUTER_PACKAGE_IMPORT
			int64_t packageNameIdx = readInt64();
			importA.packageName = resolveFName(packageNameIdx);
		}
		else {
			importA.packageName = "";
		}

		if (data.header.FileVersionUE5 >= 0x0197) { // VER_UE5_OPTIONAL_RESOURCES
			importA.bImportOptional = readInt32();
		}
		else {
			importA.bImportOptional = 0;
		}

		data.imports.push_back(importA);
	}
}

void Uasset::readExports() {
	currentIdx = data.header.ExportOffset;
	data.exports.clear();
	size_t prevCurrentIdx = currentIdx;
	for (int32_t i = 0; i < data.header.ExportCount; ++i) {
		currentIdx = prevCurrentIdx + i * 96;
		UassetData::Export exportData;
		exportData.internalIndex = i+1;
		exportData.classIndex = readInt32();
		exportData.superIndex = readInt32();

		if (data.header.FileVersionUE4 >= 0x00AD) { // VER_UE4_TEMPLATEINDEX_IN_COOKED_EXPORTS
			exportData.templateIndex = readInt32();
		}
		else {
			exportData.templateIndex = 0;
		}

		exportData.outerIndex = readInt32();
		int32_t objectNameIdx = readInt32();
		exportData.objectName = resolveFName(objectNameIdx);
		currentIdx += 4; // undocumented
		exportData.objectFlags = readUint32();
		exportData.serialSize = readInt64Export();
		exportData.serialOffset = readInt64Export();
		exportData.bForcedExport = readInt32();
		exportData.bNotForClient = readInt32();
		exportData.bNotForServer = readInt32();
		exportData.packageGuid = readGuid();
		exportData.packageFlags = readUint32();

		if (data.header.FileVersionUE4 >= 0x00AC) { // VER_UE4_LOAD_FOR_EDITOR_GAME
			exportData.bNotAlwaysLoadedForEditorGame = readInt32();
		}
		else {
			exportData.bNotAlwaysLoadedForEditorGame = 0;
		}

		if (data.header.FileVersionUE4 >= 0x00AE) { // VER_UE4_COOKED_ASSETS_IN_EDITOR_SUPPORT
			exportData.bIsAsset = readInt32();
		}
		else {
			exportData.bIsAsset = 0;
		}

		if (data.header.FileVersionUE5 >= 0x0197) { // VER_UE5_OPTIONAL_RESOURCES
			exportData.bGeneratePublicHash = readInt32();
		}
		else {
			exportData.bGeneratePublicHash = 0;
		}

		if (data.header.FileVersionUE4 >= 0x0194) { // VER_UE4_PRELOAD_DEPENDENCIES_IN_COOKED_EXPORTS
			exportData.firstExportDependency = readInt32();
			exportData.serializationBeforeSerializationDependencies = readInt32();
			exportData.createBeforeSerializationDependencies = readInt32();
			exportData.serializationBeforeCreateDependencies = readInt32();
			exportData.createBeforeCreateDependencies = readInt32();
		}
		else {
			exportData.firstExportDependency = 0;
			exportData.serializationBeforeSerializationDependencies = 0;
			exportData.createBeforeSerializationDependencies = 0;
			exportData.serializationBeforeCreateDependencies = 0;
			exportData.createBeforeCreateDependencies = 0;
		}

		// Read the export data chunk
		size_t previousIdx = currentIdx;
		currentIdx = exportData.serialOffset;
		exportData.chunkData = readCountBytes(exportData.serialSize);
		currentIdx = previousIdx; // Reset index to continue reading next export

		// Parse the export data
		readExportData(exportData);

		data.exports.push_back(exportData);
	}
}


void Uasset::readExportData(UassetData::Export& exportData) {
	size_t exportDataIdx = exportData.serialOffset;
	currentIdx = exportDataIdx;
	exportData.metadata.ObjectClass = resolveFName(readInt64());
	exportDataIdx += 8;
	exportDataIdx = exportData.serialOffset;
	currentIdx = exportDataIdx;
	if (exportData.internalIndex == 25) {
		int stop = 0;
	}
	// Loop until all data is read
	while (exportDataIdx < (size_t)(exportData.serialOffset + exportData.serialSize)) {

		std::string ObjectClass = resolveFName(readInt64());
		exportDataIdx += 8;

		// Determine the structure type for the current segment of data
		std::string structureType = determineStructureType(ObjectClass);

		// Process the data based on the structure type
		if (structureType == "ParentClass") {
			processParentClass(exportData, exportDataIdx);
		}
		else if (structureType == "DynamicBindingObjects") {
			processDynamicBindingObjects(exportData, exportDataIdx);
		}
		else if (structureType == "UberGraphFrame") {
			processUberGraphFrame(exportData, exportDataIdx);
		}
		else if (structureType == "Schema") {
			processSchema(exportData, exportDataIdx);
		}
		else if (structureType == "bCommentBubbleVisible_InDetailsPanel") {
			processbCommentBubbleVisible_InDetailsPanel(exportData, exportDataIdx);
		}
		else if (structureType == "bCommentBubbleVisible") {
			processbCommentBubbleVisible(exportData, exportDataIdx);
		}
		else if (structureType == "bCommentBubblePinned") {
			processbCommentBubblePinned(exportData, exportDataIdx);
		}
		else if (structureType == "None") {
			processNone(exportData, exportDataIdx);
		}
		else if (structureType == "InputKeyDelegateBindings") {
			processInputKeyDelegateBindings(exportData, exportDataIdx);
		}
		else if (structureType == "DelegateReference") {
			processDelegateReference(exportData, exportDataIdx);
		}
		else if (structureType == "FunctionReference") {
			processFunctionReference(exportData, exportDataIdx);
		}
		else if (structureType == "bIsPureFunc") {
			processbIsPureFunc(exportData, exportDataIdx);
		}
		else if (structureType == "bIsConstFunc") {
			processbIsConstFunc(exportData, exportDataIdx);
		}
		else if (structureType == "NodePosX") {
			processNodePosX(exportData, exportDataIdx);
		}
		else if (structureType == "NodePosY") {
			processNodePosY(exportData, exportDataIdx);
		}
		else if (structureType == "NodeWidth") {
			processNodeWidth(exportData, exportDataIdx);
		}
		else if (structureType == "NodeHeight") {
			processNodeHeight(exportData, exportDataIdx);
		}
		else if (structureType == "NodeComment") {
			processNodeComment(exportData, exportDataIdx);
		}
		else if (structureType == "CustomFunctionName") {
			processCustomFunctionName(exportData, exportDataIdx);
		}
		else if (structureType == "EventReference") {
			processEventReference(exportData, exportDataIdx);
		}
		else if (structureType == "ExtraFlags") {
			processExtraFlags(exportData, exportDataIdx);
		}
		else if (structureType == "CustomClass") {
			processCustomClass(exportData, exportDataIdx);
		}
		else if (structureType == "InputKey") {
			processInputKey(exportData, exportDataIdx);
		}
		else if (structureType == "VariableReference") {
			processVariableReference(exportData, exportDataIdx);
		}
		else if (structureType == "bVisualizeComponent") {
			processbVisualizeComponent(exportData, exportDataIdx);
		}
		else if (structureType == "ComponentClass") {
			processComponentClass(exportData, exportDataIdx);
		}
		else if (structureType == "RootNodes") {
			processRootNodes(exportData, exportDataIdx);
		}
		else if (structureType == "Nodes") {
			processNodes(exportData, exportDataIdx);
		}
		else if (structureType == "GraphGuid") {
			processGraphGuid(exportData, exportDataIdx);
		}
		else if (structureType == "NodeGuid") {
			processNodeGuid(exportData, exportDataIdx);
		}
		else if (structureType == "bAllowDeletion") {
			processbAllowDeletion(exportData, exportDataIdx);
		}
		else if (structureType == "MemberReference") {
			processMemberReference(exportData, exportDataIdx);
		}
		else if (structureType == "MemberParent") {
			processMemberParent(exportData, exportDataIdx);
		}
		else if (structureType == "MemberName") {
			processMemberName(exportData, exportDataIdx);
		}
		else if (structureType == "BlueprintSystemVersion") {
			processBlueprintSystemVersion(exportData, exportDataIdx);
		}
		else if (structureType == "SimpleConstructionScript") {
			processSimpleConstructionScript(exportData, exportDataIdx);
		}
		else if (structureType == "UbergraphPages") {
			processUbergraphPages(exportData, exportDataIdx);
		}
		else if (structureType == "FunctionGraphs") {
			processFunctionGraphs(exportData, exportDataIdx);
		}
		else if (structureType == "UberGraphFunction") {
			processUberGraphFunction(exportData, exportDataIdx);
		}
		else if (structureType == "NodeGuid") {
			processNodeGuid(exportData, exportDataIdx);
		}
		else if (structureType == "MemberGuid") {
			processMemberGuid(exportData, exportDataIdx);
		}
		else {
			break;
			processDefault(exportData, exportDataIdx);
		}

		// Update the index based on how much data was processed in the loop
		// This step is crucial to avoid infinite loops and correctly move to the next structure
		exportDataIdx = currentIdx;
	}
}

std::string Uasset::determineStructureType(const std::string& objectClass) {
	// Logic to determine the structure type based on objectClass or other metadata
	if (objectClass == "ParentClass") {
		return "ParentClass";
	}
	else if (objectClass == "DynamicBindingObjects") {
		return "DynamicBindingObjects";
	}
	else if (objectClass == "Package") {
		return "Package";
	}
	else if (objectClass == "bIsConstFunc") {
		return "bIsConstFunc";
	}
	else if (objectClass == "UberGraphFunction") {
		return "UberGraphFunction";
	}
	else if (objectClass == "UberGraphFrame") {
		return "UberGraphFrame";
	}
	else if (objectClass == "FunctionGraphs") {
		return "FunctionGraphs";
	}
	else if (objectClass == "SimpleConstructionScript") {
		return "SimpleConstructionScript";
	}
	else if (objectClass == "UbergraphPages") {
		return "UbergraphPages";
	}

	else if (objectClass == "Schema") {
		return "Schema";
	}
	else if (objectClass == "bCommentBubbleVisible_InDetailsPanel") {
		return "bCommentBubbleVisible_InDetailsPanel";
	}
	else if (objectClass == "bCommentBubbleVisible") {
		return "bCommentBubbleVisible";
	}
	else if (objectClass == "None") {
		return "None";
	}
	else if (objectClass == "InputKeyDelegateBindings") {
		return "InputKeyDelegateBindings";
	}
	else if (objectClass == "DelegateReference") {
		return "DelegateReference";
	}
	else if (objectClass == "FunctionReference") {
		return "FunctionReference";
	}
	else if (objectClass == "bIsPureFunc") {
		return "bIsPureFunc";
	}
	else if (objectClass == "NodePosX") {
		return "NodePosX";
	}
	else if (objectClass == "NodePosY") {
		return "NodePosY";
	}
	else if (objectClass == "NodeWidth") {
		return "NodeWidth";
	}
	else if (objectClass == "NodeHeight") {
		return "NodeHeight";
	}
	else if (objectClass == "NodeComment") {
		return "NodeComment";
	}
	else if (objectClass == "CustomFunctionName") {
		return "CustomFunctionName";
	}
	else if (objectClass == "EventReference") {
		return "EventReference";
	}
	else if (objectClass == "ExtraFlags") {
		return "ExtraFlags";
	}
	else if (objectClass == "CustomClass") {
		return "CustomClass";
	}
	else if (objectClass == "InputKey") {
		return "InputKey";
	}
	else if (objectClass == "VariableReference") {
		return "VariableReference";
	}
	else if (objectClass == "bVisualizeComponent") {
		return "bVisualizeComponent";
	}
	else if (objectClass == "ComponentClass") {
		return "ComponentClass";
	}
	else if (objectClass == "RootNodes") {
		return "RootNodes";
	}
	else if (objectClass == "Nodes") {
		return "Nodes";
	}
	else if (objectClass == "GraphGuid") {
		return "GraphGuid";
	}
	else if (objectClass == "NodeGuid") {
		return "NodeGuid";
	}
	else if (objectClass == "MemberGuid") {
		return "MemberGuid";
	}
	else if (objectClass == "bAllowDeletion") {
		return "bAllowDeletion";
	}
	else if (objectClass == "bCommentBubblePinned") {
		return "bCommentBubblePinned";
	}
	else if (objectClass == "MemberReference") {
		return "MemberReference";
	}
	else if (objectClass == "MemberParent") {
		return "MemberParent";
	}
	else if (objectClass == "MemberName") {
		return "MemberName";
	}
	else if (objectClass == "BlueprintSystemVersion") {
		return "BlueprintSystemVersion";
	}
	else {
		return "Unknown";
	}
}


void Uasset::processParentClass(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Specific logic for processing structures
		// Read and process fields specific

	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;
	readByte(); 
	exportDataIdx += 1;
	readInt32();
	exportDataIdx += 4;
}


void Uasset::processDynamicBindingObjects(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Specific logic for processing  structures
		// Read and process fields specific
		// Example:
	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;

	readInt64();
	exportDataIdx += 8;

	readByte();
	exportDataIdx += 1;

	UassetData::Export::Property property;
	property.PropertyName = "DynamicBindingObjects";
	property.PropertyType = "int";
	int count = readInt32();
	property.intValue = count;
	exportData.properties.push_back(property);
	exportDataIdx += 4;

	for (int i = 0; i < count; i++) {
		property.PropertyName = "DynamicBindingObject[" + std::to_string(i) + "]";
		property.PropertyType = "int";
		property.intValue = readInt32();
		exportData.properties.push_back(property);
		exportDataIdx += 4;
	}

}


void Uasset::processUberGraphFrame(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Specific logic for processing  structures
		// Read and process fields specific
		// Example:
	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;

	UassetData::Export::Property property;
	property.PropertyName = "UberGraphFrame";
	property.PropertyType = "int";
	property.intValue = readInt32();
	exportDataIdx += 4;
	exportData.properties.push_back(property);

	// Check for end marker
	if (property.PropertyName == "None") {
	}
}

void Uasset::processSchema(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Specific logic for processing  structures
	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;

	// move forward 5 bytes 
	readByte();
	readInt32();
	exportDataIdx += 5;
}
void Uasset::processbCommentBubbleVisible_InDetailsPanel(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Specific logic for processing  structures
		// Read and process fields specific
		// Example:
	exportData.metadata.ObjectName = resolveFName(readInt64()); // class type 
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64()); // class value
	exportDataIdx += 8;

	readByte();
	readByte();

	// Check for end marker
}
void Uasset::processbCommentBubbleVisible(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Specific logic for processing  structures
		// Read and process fields specific
		// Example:
	exportData.metadata.ObjectName = resolveFName(readInt64()); // class type 
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64()); // class value
	exportDataIdx += 8;

	readByte();
	readByte();

	// Check for end marker
}
void Uasset::processbCommentBubblePinned(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Specific logic for processing  structures
		// Read and process fields specific
		// Example:
	exportData.metadata.ObjectName = resolveFName(readInt64()); // class type 
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64()); // class value
	exportDataIdx += 8;

	readByte();
	readByte();

	// Check for end marker
}



void Uasset::processNone(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Specific logic for processing  structures
		// Read and process fields specific
		// Example:

	UassetData::Export::Property property;
	//property.PropertyName = resolveFName(readInt32());
	//exportDataIdx += 4;

	//// Add more logic specific ...

	//// Add the property to the export's properties vector
	//exportData.properties.push_back(property);

	//// Check for end marker
	//if (property.PropertyName == "None") {
	//}
}

void Uasset::processInputKeyDelegateBindings(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Specific logic for processing  structures
		// Read and process fields specific
		// Example:
	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;

	UassetData::Export::Property property;
	property.PropertyName = resolveFName(readInt32());
	exportDataIdx += 4;

	// Add more logic specific ...

	// Add the property to the export's properties vector
	exportData.properties.push_back(property);

	// Check for end marker
	if (property.PropertyName == "None") {
	}
}

void Uasset::processDelegateReference(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Specific logic for processing  structures
		// Read and process fields specific
		// Example:
	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;
	readInt64();
	readByte();
	readInt64(); // read 8 zeros
	readInt64(); // read 8 zeros
}


void Uasset::processMemberReference(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Specific logic for processing  structures
		// Read and process fields specific
		// Example:
	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;
	bool value = readByte();
	exportDataIdx += 1;
}


void Uasset::processMemberParent(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Specific logic for processing  structures
		// Read and process fields specific
		// Example:
	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;
	bool value = readByte();
	exportDataIdx += 1;
	
	UassetData::Export::Property property;
	property.PropertyName = "MemberParent(value)";
	property.PropertyType = "int";
	property.intValue = readInt32();
	exportDataIdx += 4;
	exportData.properties.push_back(property);
}

void Uasset::processMemberName(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Specific logic for processing  structures
		// Read and process fields specific
		// Example:
	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;
	bool value = readByte();
	exportDataIdx += 1;

	UassetData::Export::Property property;
	property.PropertyName = "MemberName(value)";
	property.PropertyType = "FString";
	property.stringValue = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.properties.push_back(property);
}


void Uasset::processBlueprintSystemVersion(UassetData::Export& exportData, size_t& exportDataIdx) {

	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;
	readByte();
	exportDataIdx += 1;
	UassetData::Export::Property property;
	property.PropertyName = "BlueprintSystemVersion";
	property.PropertyType = "int";
	property.intValue = readInt32();
	exportDataIdx += 4;
	exportData.properties.push_back(property);
}

void Uasset::processSimpleConstructionScript(UassetData::Export& exportData, size_t& exportDataIdx) {

	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;
	readByte();
	exportDataIdx += 1;
	UassetData::Export::Property property;
	property.PropertyName = "SimpleConstructionScript";
	property.PropertyType = "int";
	property.intValue = readInt32();
	exportDataIdx += 4;
	exportData.properties.push_back(property);
}

void Uasset::processUbergraphPages(UassetData::Export& exportData, size_t& exportDataIdx) {

	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;
	readInt64();
	exportDataIdx += 8;
	readByte();
	exportDataIdx += 1;

	UassetData::Export::Property property;
	property.PropertyName = "UbergraphPages";
	property.PropertyType = "int";
	int count = readInt32();
	property.intValue = count;
	exportData.properties.push_back(property);
	exportDataIdx += 4;

	for (int i = 0; i < count; i++) {
		property.PropertyName = "UbergraphPage[" + std::to_string(i) + "]";
		property.PropertyType = "int";
		property.intValue = readInt32();
		exportData.properties.push_back(property);
		exportDataIdx += 4;
	}
}

void Uasset::processUberGraphFunction(UassetData::Export& exportData, size_t& exportDataIdx) {

	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;
	readInt64();
	exportDataIdx += 8;
	readByte();
	exportDataIdx += 1;

	UassetData::Export::Property property;
	property.PropertyName = "UbergraphPages";
	property.PropertyType = "int";
	int count = readInt32();
	property.intValue = count;
	exportData.properties.push_back(property);
	exportDataIdx += 4;

	for (int i = 0; i < count; i++) {
		property.PropertyName = "UbergraphPage[" + std::to_string(i) + "]";
		property.PropertyType = "int";
		property.intValue = readInt32();
		exportData.properties.push_back(property);
		exportDataIdx += 4;
	}
}

void Uasset::processPackage(UassetData::Export& exportData, size_t& exportDataIdx) {

	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;
	readInt64();
	exportDataIdx += 8;
	readInt64();
	exportDataIdx += 8;
	readInt64();
	exportDataIdx += 8;
	readInt32();
	exportDataIdx += 4;

}

void Uasset::processFunctionGraphs(UassetData::Export& exportData, size_t& exportDataIdx) {

	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;
	readInt64();
	exportDataIdx += 8;
	readByte();
	exportDataIdx += 1;

	UassetData::Export::Property property;
	property.PropertyName = "FunctionGraphs";
	property.PropertyType = "int";
	int count = readInt32();
	property.intValue = count;
	exportData.properties.push_back(property);
	exportDataIdx += 4;

	for (int i = 0; i < count; i++) {
		property.PropertyName = "FunctionGraphs[" + std::to_string(i) + "]";
		property.PropertyType = "int";
		property.intValue = readInt32();
		exportData.properties.push_back(property);
		exportDataIdx += 4;
	}
}

void Uasset::processFunctionReference(UassetData::Export& exportData, size_t& exportDataIdx) {

	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;

	readInt64();
	exportDataIdx += 8; // read MemberReference
	readInt64();
	exportDataIdx += 8;// read zero values
	readInt64();
	exportDataIdx += 8;// read zero values
	readByte();
	exportDataIdx += 1;
}

void Uasset::processbIsPureFunc(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Specific logic for processing  structures
		// Read and process fields specific
		// Example:
	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;

	readByte();
	exportDataIdx += 1;
	readByte();
	exportDataIdx += 1;

}

void Uasset::processbIsConstFunc(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Specific logic for processing  structures
		// Read and process fields specific
		// Example:
	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;

	readByte();
	exportDataIdx += 1;
	readByte();
	exportDataIdx += 1;

}


void Uasset::processNodePosX(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Specific logic for processing  structures
		// Read and process fields specific
		// Example:
	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;
	UassetData::Export::Property property;

	readByte();
	exportDataIdx += 1;

	property.PropertyName = "NodePosX";
	property.PropertyType = "int";
	property.intValue = readInt32();
	exportData.properties.push_back(property);
	exportDataIdx += 4;
}

void Uasset::processNodePosY(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Specific logic for processing  structures
		// Read and process fields specific
		// Example:
	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;

	UassetData::Export::Property property;

	readByte();
	exportDataIdx += 1;

	property.PropertyName = "NodePosY";
	property.PropertyType = "int";
	property.intValue = readInt32();
	exportData.properties.push_back(property);
	exportDataIdx += 4;
}

void Uasset::processNodeWidth(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Specific logic for processing  structures
		// Read and process fields specific
		// Example:
	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;

	UassetData::Export::Property property;

	readByte();
	exportDataIdx += 1;

	property.PropertyName = "NodeWidth";
	property.PropertyType = "int";
	property.intValue = readInt32();
	exportData.properties.push_back(property);
	exportDataIdx += 4;

}


void Uasset::processNodeHeight(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Specific logic for processing  structures
		// Read and process fields specific
		// Example:
	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;

	UassetData::Export::Property property;

	readByte();
	exportDataIdx += 1;

	property.PropertyName = "NodeHeight";
	property.PropertyType = "int";
	property.intValue = readInt32();
	exportData.properties.push_back(property);
	exportDataIdx += 4;
}

void Uasset::processNodeComment(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Specific logic for processing  structures
		// Read and process fields specific
		// Example:
	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;

	UassetData::Export::Property property;

	readByte();
	exportDataIdx += 1;

	property.PropertyName = "NodeComment";
	property.PropertyType = "FString";
	property.stringValue = readFString();
	exportData.properties.push_back(property);
	exportDataIdx += 4;
}

void Uasset::processCustomFunctionName(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Specific logic for processing  structures
		// Read and process fields specific
		// Example:
	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;

	UassetData::Export::Property property;
	property.PropertyName = resolveFName(readInt32());
	exportDataIdx += 4;

	// Add more logic specific ...

	// Add the property to the export's properties vector
	exportData.properties.push_back(property);

	// Check for end marker
	if (property.PropertyName == "None") {
	}
}

void Uasset::processEventReference(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Specific logic for processing  structures
		// Read and process fields specific
		// Example:
	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;

	UassetData::Export::Property property;
	property.PropertyName = resolveFName(readInt32());
	exportDataIdx += 4;

	// Add more logic specific ...

	// Add the property to the export's properties vector
	exportData.properties.push_back(property);

	// Check for end marker
	if (property.PropertyName == "None") {
	}
}

void Uasset::processExtraFlags(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Specific logic for processing  structures
		// Read and process fields specific
		// Example:
	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;

	UassetData::Export::Property property;
	property.PropertyName = resolveFName(readInt32());
	exportDataIdx += 4;

	// Add more logic specific ...

	// Add the property to the export's properties vector
	exportData.properties.push_back(property);

	// Check for end marker
	if (property.PropertyName == "None") {
	}
}


void Uasset::processCustomClass(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Specific logic for processing  structures
		// Read and process fields specific
		// Example:
	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;

	UassetData::Export::Property property;
	property.PropertyName = resolveFName(readInt32());
	exportDataIdx += 4;

	// Add more logic specific ...

	// Add the property to the export's properties vector
	exportData.properties.push_back(property);

	// Check for end marker
	if (property.PropertyName == "None") {
	}
}

void Uasset::processInputKey(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Specific logic for processing  structures
		// Read and process fields specific
		// Example:
	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;

	UassetData::Export::Property property;
	property.PropertyName = resolveFName(readInt32());
	exportDataIdx += 4;

	// Add more logic specific ...

	// Add the property to the export's properties vector
	exportData.properties.push_back(property);

	// Check for end marker
	if (property.PropertyName == "None") {
	}
}

void Uasset::processVariableReference(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Specific logic for processing  structures
		// Read and process fields specific
		// Example:
	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;

	UassetData::Export::Property property;
	property.PropertyName = resolveFName(readInt32());
	exportDataIdx += 4;

	// Add more logic specific ...

	// Add the property to the export's properties vector
	exportData.properties.push_back(property);

	// Check for end marker
	if (property.PropertyName == "None") {
	}
}

void Uasset::processbVisualizeComponent(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Specific logic for processing  structures
		// Read and process fields specific
		// Example:
	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;

	UassetData::Export::Property property;
	property.PropertyName = resolveFName(readInt32());
	exportDataIdx += 4;

	// Add more logic specific ...

	// Add the property to the export's properties vector
	exportData.properties.push_back(property);

	// Check for end marker
	if (property.PropertyName == "None") {
	}
}

void Uasset::processComponentClass(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Specific logic for processing  structures
	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;

	// Read and process fields specific
		// Example:
		UassetData::Export::Property property;
		property.PropertyName = resolveFName(readInt32());
		exportDataIdx += 4;

		// Add more logic specific ...

		// Add the property to the export's properties vector
		exportData.properties.push_back(property);

		// Check for end marker
		if (property.PropertyName == "None") {
		}
}

void Uasset::processRootNodes(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Specific logic for processing  structures
		// Read and process fields specific
		// Example:
	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;

	UassetData::Export::Property property;
	property.PropertyName = resolveFName(readInt32());
	exportDataIdx += 4;

	// Add more logic specific ...

	// Add the property to the export's properties vector
	exportData.properties.push_back(property);

	// Check for end marker
	if (property.PropertyName == "None") {
	}
}

void Uasset::processNodes(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Example:
	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;

	resolveFName(readInt64());
	exportDataIdx += 8;
	readByte();
	exportDataIdx += 1;

	UassetData::Export::Property property;
	property.PropertyName = "NumberOfNodes";
	property.PropertyType = "int";
	int count = readInt32();
	property.intValue = count;
	exportData.properties.push_back(property);
	exportDataIdx += 4;

	for (int i = 0; i < count; i++) {
		property.PropertyName = "Node["+ std::to_string(i)+"]";
		property.PropertyType = "int";
		property.intValue = readInt32();
		exportData.properties.push_back(property);
		exportDataIdx += 4;

	}
}


void Uasset::processGraphGuid(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Example:

	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;

	UassetData::Export::Property property;
	property.PropertyName = resolveFName(readInt64()); //Guid 
	exportDataIdx += 8;
	std::string unknown1 = resolveFName(readInt64());
	exportDataIdx += 8;
	std::string unknown2 = resolveFName(readInt64());
	exportDataIdx += 8;
	readByte();
	exportDataIdx += 1;

	property.PropertyType = "FString";
	property.stringValue = readGuid();
	exportDataIdx += 16;
	exportData.properties.push_back(property);
}

void Uasset::processNodeGuid(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Example:

	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;

	UassetData::Export::Property property;
	property.PropertyName = resolveFName(readInt64()); //Guid 
	exportDataIdx += 8;
	std::string unknown1 = resolveFName(readInt64());
	exportDataIdx += 8;
	std::string unknown2 = resolveFName(readInt64());
	exportDataIdx += 8;
	readByte();
	exportDataIdx += 1;

	property.PropertyType = "FString";
	property.stringValue = readGuid();
	exportDataIdx += 16;
	exportData.properties.push_back(property);
}

void Uasset::processMemberGuid(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Example:

	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;

	UassetData::Export::Property property;
	property.PropertyName = resolveFName(readInt64()); //Guid 
	exportDataIdx += 8;
	std::string unknown1 = resolveFName(readInt64());
	exportDataIdx += 8;
	std::string unknown2 = resolveFName(readInt64());
	exportDataIdx += 8;
	readByte();
	exportDataIdx += 1;

	property.PropertyType = "FString";
	property.stringValue = readGuid();
	exportDataIdx += 16;
	exportData.properties.push_back(property);
}


void Uasset::processbAllowDeletion(UassetData::Export& exportData, size_t& exportDataIdx) {
	// Example:
	exportData.metadata.ObjectName = resolveFName(readInt64());
	exportDataIdx += 8;
	exportData.metadata.OuterObject = resolveFName(readInt64());
	exportDataIdx += 8;


	readByte();
	exportDataIdx += 1;
	readByte();
	exportDataIdx += 1;

}

void Uasset::processDefault(UassetData::Export& exportData, size_t& exportDataIdx) {
	// move forward one byte
	readByte(); 
	exportDataIdx += 1;

}


float Uasset::readFloat() {
	if (currentIdx + sizeof(float) > bytesPtr->size()) {
		throw ParseException("Out of bounds read (float)");
	}
	float val;
	std::memcpy(&val, &(*bytesPtr)[currentIdx], sizeof(val));
	currentIdx += sizeof(val);
	return val;
}

bool Uasset::readBool() {
	if (currentIdx + sizeof(uint8_t) > bytesPtr->size()) {
		throw ParseException("Out of bounds read (bool)");
	}
	uint8_t val = (*bytesPtr)[currentIdx];
	currentIdx += sizeof(uint8_t);
	return val != 0;
}



void Uasset::readThumbnails() {
	currentIdx = data.header.ThumbnailTableOffset;

	int32_t count = readInt32();
	data.thumbnailsIndex.clear();
	data.thumbnails.clear();

	for (int32_t idx = 0; idx < count; ++idx) {
		ThumbnailIndex index;
		index.AssetClassName = readFString();
		index.ObjectPathWithoutPackageName = readFString();
		index.FileOffset = readInt32();
		data.thumbnailsIndex.push_back(index);
	}

	for (int32_t idx = 0; idx < count; ++idx) {
		currentIdx = data.thumbnailsIndex[idx].FileOffset;

		Thumbnail thumbnail;
		thumbnail.ImageWidth = readInt32();
		thumbnail.ImageHeight = readInt32();
		thumbnail.ImageFormat = "PNG";

		if (thumbnail.ImageHeight < 0) {
			thumbnail.ImageFormat = "JPEG";
			thumbnail.ImageHeight = -thumbnail.ImageHeight;
		}

		thumbnail.ImageSizeData = readInt32();
		if (thumbnail.ImageSizeData > 0) {
			thumbnail.ImageData = readCountBytes(thumbnail.ImageSizeData);
		}

		data.thumbnails.push_back(thumbnail);
	}
}

void Uasset::readAssetRegistryData() {
	currentIdx = data.header.AssetRegistryDataOffset;

	int32_t nextOffset = data.header.TotalHeaderSize;
	if (data.header.WorldTileInfoDataOffset > 0) {
		nextOffset = data.header.WorldTileInfoDataOffset;
	}

	data.assetRegistryData.size = nextOffset - data.header.AssetRegistryDataOffset;

	data.assetRegistryData.DependencyDataOffset = readInt64();

	int32_t count = readInt32();
	data.assetRegistryData.data.clear();
	for (int32_t idx = 0; idx < count; ++idx) {
		AssetRegistryEntry entry;
		entry.ObjectPath = readFString();
		entry.ObjectClassName = readFString();

		int32_t countTag = readInt32();
		for (int32_t idxTag = 0; idxTag < countTag; ++idxTag) {
			Tag tag;
			tag.Key = readFString();
			tag.Value = readFString();
			entry.Tags.push_back(tag);
		}

		data.assetRegistryData.data.push_back(entry);
	}
}

std::vector<uint8_t> Uasset::readCountBytes(int64_t count) {
	if (currentIdx + count > bytesPtr->size()) {
		throw std::runtime_error("Out of bounds read (count bytes)");
	}
	std::vector<uint8_t> bytes((*bytesPtr).begin() + currentIdx, (*bytesPtr).begin() + currentIdx + count);
	currentIdx += count;
	return bytes;
}

uint16_t Uasset::readUint16() {
	if (currentIdx + sizeof(uint16_t) > bytesPtr->size()) {
		throw ParseException("Out of bounds read (uint16)");
	}
	uint16_t val;
	std::memcpy(&val, &(*bytesPtr)[currentIdx], sizeof(val));
	currentIdx += sizeof(val);
	return val;
}

int32_t Uasset::readInt32() {
	if (currentIdx + sizeof(int32_t) > bytesPtr->size()) {
		throw ParseException("Out of bounds read (int32)");
	}
	int32_t val;
	std::memcpy(&val, &(*bytesPtr)[currentIdx], sizeof(val));
	currentIdx += sizeof(val);
	return val;
}

uint32_t Uasset::readUint32() {
	if (currentIdx + sizeof(uint32_t) > bytesPtr->size()) {
		throw ParseException("Out of bounds read (uint32)");
	}
	uint32_t val;
	std::memcpy(&val, &(*bytesPtr)[currentIdx], sizeof(val));
	currentIdx += sizeof(val);
	return val;
}

int64_t Uasset::readInt64() {
	if (currentIdx + sizeof(int64_t) > bytesPtr->size()) {
		throw ParseException("Out of bounds read (int64)");
	}
	int64_t val;
	std::memcpy(&val, &(*bytesPtr)[currentIdx], sizeof(val));
	currentIdx += sizeof(val);
	return val;
}

int64_t Uasset::readInt64Export() {
	if (currentIdx + sizeof(int64_t) > bytesPtr->size()) {
		throw std::runtime_error("Out of bounds read (int64)");
	}
	uint8_t b0 = (*bytesPtr)[currentIdx];
	uint8_t b1 = (*bytesPtr)[currentIdx + 1];
	uint8_t b2 = (*bytesPtr)[currentIdx + 2];
	uint8_t b3 = (*bytesPtr)[currentIdx + 3];
	uint8_t b4 = (*bytesPtr)[currentIdx + 4];
	uint8_t b5 = (*bytesPtr)[currentIdx + 5];
	uint8_t b6 = (*bytesPtr)[currentIdx + 6];
	uint8_t b7 = (*bytesPtr)[currentIdx + 7];
	currentIdx += sizeof(int64_t);
	return (int64_t(b0) | (int64_t(b1) << 8) | (int64_t(b2) << 16) | (int64_t(b3) << 24) |
		(int64_t(b4) << 32) | (int64_t(b5) << 40) | (int64_t(b6) << 48) | (int64_t(b7) << 56));
}

std::string Uasset::readFString() {
	int32_t length = readInt32();
	if (length == 0) return "";
	if (length > 0) {
		if (currentIdx + length > bytesPtr->size()) {
			throw ParseException("Out of bounds read (FString)");
		}
		std::string str((*bytesPtr).begin() + currentIdx, (*bytesPtr).begin() + currentIdx + length - 1);
		currentIdx += length;
		return str;
	}
	else {
		length = -length * 2;
		if (currentIdx + length > bytesPtr->size()) {
			throw ParseException("Out of bounds read (FString)");
		}
		std::wstring wstr((wchar_t*)(&(*bytesPtr)[currentIdx]), length / 2 - 1);
		currentIdx += length;
		std::string result;
		result.reserve(wstr.size());
		for (wchar_t wc : wstr) {
			result.push_back(static_cast<char>(wc));
		}
		return result;
	}
}

//std::string Uasset::readGuid() {
//	uint8_t guid[16];
//	if (currentIdx + sizeof(guid) > bytesPtr->size()) {
//		throw ParseException("Out of bounds read (Guid)");
//	}
//	std::memcpy(guid, &(*bytesPtr)[currentIdx], sizeof(guid));
//	currentIdx += sizeof(guid);
//	return guidToString(guid);
//}

std::string Uasset::readGuid() {
	uint8_t guid[16];
	if (currentIdx + sizeof(guid) > bytesPtr->size()) {
		throw ParseException("Out of bounds read (Guid)");
	}
	std::memcpy(guid, &(*bytesPtr)[currentIdx], sizeof(guid));
	currentIdx += sizeof(guid);

	// Convert the GUID to the correct string format
	std::ostringstream ss;
	ss << std::hex << std::setfill('0');

	// Handle endianess
	ss << std::setw(2) << static_cast<int>(guid[3])
		<< std::setw(2) << static_cast<int>(guid[2])
		<< std::setw(2) << static_cast<int>(guid[1])
		<< std::setw(2) << static_cast<int>(guid[0]) << '-';

	ss << std::setw(2) << static_cast<int>(guid[5])
		<< std::setw(2) << static_cast<int>(guid[4]) << '-';

	ss << std::setw(2) << static_cast<int>(guid[7])
		<< std::setw(2) << static_cast<int>(guid[6]) << '-';

	ss << std::setw(2) << static_cast<int>(guid[8])
		<< std::setw(2) << static_cast<int>(guid[9]) << '-';

	for (int i = 10; i < 16; ++i) {
		ss << std::setw(2) << static_cast<int>(guid[i]);
	}

	return ss.str();
}


std::string Uasset::resolveFName(int64_t idx) {
	if (idx >= 0 && idx < (int64_t)data.names.size()) {
		return data.names[idx].Name;
	}
	return "";
}

json Uasset::toJson() const {
	json j;
	j["header"] = {
		{"EPackageFileTag", data.header.EPackageFileTag},
		{"LegacyFileVersion", data.header.LegacyFileVersion},
		{"LegacyUE3Version", data.header.LegacyUE3Version},
		{"FileVersionUE4", data.header.FileVersionUE4},
		{"FileVersionUE5", data.header.FileVersionUE5},
		{"FileVersionLicenseeUE4", data.header.FileVersionLicenseeUE4},
		{"CustomVersions", data.header.CustomVersions},
		{"TotalHeaderSize", data.header.TotalHeaderSize},
		{"FolderName", data.header.FolderName},
		{"PackageFlags", data.header.PackageFlags},
		{"NameCount", data.header.NameCount},
		{"NameOffset", data.header.NameOffset},
		{"SoftObjectPathsCount", data.header.SoftObjectPathsCount},
		{"SoftObjectPathsOffset", data.header.SoftObjectPathsOffset},
		{"LocalizationId", data.header.LocalizationId},
		{"GatherableTextDataCount", data.header.GatherableTextDataCount},
		{"GatherableTextDataOffset", data.header.GatherableTextDataOffset},
		{"ExportCount", data.header.ExportCount},
		{"ExportOffset", data.header.ExportOffset},
		{"ImportCount", data.header.ImportCount},
		{"ImportOffset", data.header.ImportOffset},
		{"DependsOffset", data.header.DependsOffset},
		{"SoftPackageReferencesCount", data.header.SoftPackageReferencesCount},
		{"SoftPackageReferencesOffset", data.header.SoftPackageReferencesOffset},
		{"SearchableNamesOffset", data.header.SearchableNamesOffset},
		{"ThumbnailTableOffset", data.header.ThumbnailTableOffset},
		{"Guid", data.header.Guid},
		{"PersistentGuid", data.header.PersistentGuid},
		{"OwnerPersistentGuid", data.header.OwnerPersistentGuid},
		{"Generations", data.header.Generations},
		{"SavedByEngineVersion", data.header.SavedByEngineVersion},
		{"CompatibleWithEngineVersion", data.header.CompatibleWithEngineVersion},
		{"CompressionFlags", data.header.CompressionFlags},
		{"PackageSource", data.header.PackageSource},
		{"AdditionalPackagesToCookCount", data.header.AdditionalPackagesToCookCount},
		{"NumTextureAllocations", data.header.NumTextureAllocations},
		{"AssetRegistryDataOffset", data.header.AssetRegistryDataOffset},
		{"BulkDataStartOffset", data.header.BulkDataStartOffset},
		{"WorldTileInfoDataOffset", data.header.WorldTileInfoDataOffset},
		{"ChunkIDs", data.header.ChunkIDs},
		{"ChunkID", data.header.ChunkID},
		{"PreloadDependencyCount", data.header.PreloadDependencyCount},
		{"PreloadDependencyOffset", data.header.PreloadDependencyOffset},
		{"NamesReferencedFromExportDataCount", data.header.NamesReferencedFromExportDataCount},
		{"PayloadTocOffset", data.header.PayloadTocOffset},
		{"DataResourceOffset", data.header.DataResourceOffset},
		{"EngineChangelist", data.header.EngineChangelist}
	};
	j["names"] = json::array();
	for (const auto& name : data.names) {
		j["names"].push_back({
			{"Name", name.Name},
			{"NonCasePreservingHash", name.NonCasePreservingHash},
			{"CasePreservingHash", name.CasePreservingHash}
			});
	}
	j["imports"] = json::array();
	for (const auto & import : data.imports) {
		j["imports"].push_back({
			{"classPackage", import.classPackage},
			{"className", import.className},
			{"outerIndex", import.outerIndex},
			{"objectName", import.objectName},
			{"packageName", import.packageName},
			{"bImportOptional", import.bImportOptional}
			});
	}
	j["exports"] = json::array();
	for (const auto& exportData : data.exports) {
		j["exports"].push_back({
			{"classIndex", exportData.classIndex},
			{"superIndex", exportData.superIndex},
			{"templateIndex", exportData.templateIndex},
			{"outerIndex", exportData.outerIndex},
			{"objectName", exportData.objectName},
			{"objectFlags", exportData.objectFlags},
			{"serialSize", exportData.serialSize},
			{"serialOffset", exportData.serialOffset},
			{"bForcedExport", exportData.bForcedExport},
			{"bNotForClient", exportData.bNotForClient},
			{"bNotForServer", exportData.bNotForServer},
			{"packageGuid", exportData.packageGuid},
			{"packageFlags", exportData.packageFlags},
			{"bNotAlwaysLoadedForEditorGame", exportData.bNotAlwaysLoadedForEditorGame},
			{"bIsAsset", exportData.bIsAsset},
			{"bGeneratePublicHash", exportData.bGeneratePublicHash},
			{"firstExportDependency", exportData.firstExportDependency},
			{"serializationBeforeSerializationDependencies", exportData.serializationBeforeSerializationDependencies},
			{"createBeforeSerializationDependencies", exportData.createBeforeSerializationDependencies},
			{"serializationBeforeCreateDependencies", exportData.serializationBeforeCreateDependencies},
			{"createBeforeCreateDependencies", exportData.createBeforeCreateDependencies},
			{"data", exportData.data}
			});
	}
	j["thumbnails"] = json::array();
	for (const auto& thumbnail : data.thumbnails) {
		j["thumbnails"].push_back({
			{"ImageWidth", thumbnail.ImageWidth},
			{"ImageHeight", thumbnail.ImageHeight},
			{"ImageFormat", thumbnail.ImageFormat},
			{"ImageSizeData", thumbnail.ImageSizeData},
			{"ImageData", thumbnail.ImageData}
			});
	}
	j["assetRegistryData"] = {
		{"DependencyDataOffset", data.assetRegistryData.DependencyDataOffset},
		{"size", data.assetRegistryData.size},
		{"data", json::array()}
	};
	for (const auto& entry : data.assetRegistryData.data) {
		json entryJson = {
			{"ObjectPath", entry.ObjectPath},
			{"ObjectClassName", entry.ObjectClassName},
			{"Tags", json::array()}
		};
		for (const auto& tag : entry.Tags) {
			entryJson["Tags"].push_back({
				{"Key", tag.Key},
				{"Value", tag.Value}
				});
		}
		j["assetRegistryData"]["data"].push_back(entryJson);
	}
	return j;
}

std::string resolveFNameE(const UassetData& data, int32_t idx) {
	if (idx >= 0 && idx < data.names.size()) {
		return data.names[idx].Name;
	}
	return "";
}

void printUassetData(const UassetData& data) {
	std::cout << "Header: " << data.header.EPackageFileTag << std::endl;
	std::cout << "Number of names: " << data.names.size() << std::endl;
	std::cout << "Number of imports: " << data.imports.size() << std::endl;
	std::cout << "Number of exports: " << data.exports.size() << std::endl;

	// Print names
	for (const auto& name : data.names) {
		std::cout << std::endl;
		std::cout << "Name: " << name.Name << std::endl;
		std::cout << "NonCasePreservingHash: " << name.NonCasePreservingHash << std::endl;
		std::cout << "CasePreservingHash: " << name.CasePreservingHash << std::endl;
	}

	// Print imports
	std::cout << "Imports:" << std::endl;
	for (size_t i = 0; i < data.imports.size(); ++i) {
		const auto& importA = data.imports[i];
		std::cout << "Import #" << (i + 1) << ":" << std::endl;
		std::cout << "  classPackage: " << importA.classPackage << std::endl;
		std::cout << "  className: " << importA.className << std::endl;
		std::cout << "  outerIndex: " << importA.outerIndex << std::endl;
		std::cout << "  objectName: " << importA.objectName << std::endl;
		std::cout << "  packageName: " << importA.packageName << std::endl;
		std::cout << "  bImportOptional: " << importA.bImportOptional << std::endl;
	}

	// Print exports
	std::cout << "Exports:" << std::endl;
	for (size_t i = 0; i < data.exports.size(); ++i) {
		const auto& exportA = data.exports[i];
		std::cout << "Export #" << (i + 1) << ":" << std::endl;
		std::cout << "  classIndex: " << exportA.classIndex << std::endl;
		std::cout << "  superIndex: " << exportA.superIndex << std::endl;
		std::cout << "  templateIndex: " << exportA.templateIndex << std::endl;
		std::cout << "  outerIndex: " << exportA.outerIndex << std::endl;
		std::cout << "  objectName: " << exportA.objectName << std::endl;
		std::cout << "  objectFlags: " << exportA.objectFlags << std::endl;
		std::cout << "  serialSize: " << exportA.serialSize << std::endl;
		std::cout << "  serialOffset: " << exportA.serialOffset << std::endl;
		std::cout << "  bForcedExport: " << exportA.bForcedExport << std::endl;
		std::cout << "  bNotForClient: " << exportA.bNotForClient << std::endl;
		std::cout << "  bNotForServer: " << exportA.bNotForServer << std::endl;
		std::cout << "  packageGuid: " << exportA.packageGuid << std::endl;
		std::cout << "  packageFlags: " << exportA.packageFlags << std::endl;
		std::cout << "  bNotAlwaysLoadedForEditorGame: " << exportA.bNotAlwaysLoadedForEditorGame << std::endl;
		std::cout << "  bIsAsset: " << exportA.bIsAsset << std::endl;
		std::cout << "  bGeneratePublicHash: " << exportA.bGeneratePublicHash << std::endl;
		std::cout << "  firstExportDependency: " << exportA.firstExportDependency << std::endl;
		std::cout << "  serializationBeforeSerializationDependencies: " << exportA.serializationBeforeSerializationDependencies << std::endl;
		std::cout << "  createBeforeSerializationDependencies: " << exportA.createBeforeSerializationDependencies << std::endl;
		std::cout << "  serializationBeforeCreateDependencies: " << exportA.serializationBeforeCreateDependencies << std::endl;
		std::cout << "  createBeforeCreateDependencies: " << exportA.createBeforeCreateDependencies << std::endl;
		for (size_t j = 0; j < exportA.data.size(); ++j) {
			std::cout << "  data[" << j << "]: " << exportA.data[j] << std::endl;
		}
		// Print nested serial data
		std::cout << "  Export Serial Data (Chunk):" << std::endl;

		// Interpret the serial data as per the provided structure.
		if (exportA.chunkData.size() > 0) {
			const uint8_t* dataPtr = exportA.chunkData.data();
			std::cout << "    ObjectMetadata:" << std::endl;
			std::cout << "      ObjectClass: " << exportA.metadata.ObjectClass << std::endl;
			std::cout << "      ObjectName: " << exportA.metadata.ObjectName << std::endl;
			std::cout << "      OuterObject: " << exportA.metadata.OuterObject << std::endl;

			std::cout << "    ObjectProperties:" << std::endl;
			for (size_t j = 0; j < exportA.properties.size(); ++j) {
			    std::cout << "      Name: " << exportA.properties.at(j).PropertyName << "     ";
			    std::cout << " (" << exportA.properties.at(j).PropertyType << ") ";
				if (exportA.properties.at(j).PropertyType == "bool") {
					std::cout << " " << exportA.properties.at(j).boolValue << " ";
				}
				else if (exportA.properties.at(j).PropertyType == "int") {
					std::cout << " " << exportA.properties.at(j).intValue << " ";
				}
				else if (exportA.properties.at(j).PropertyType == "float") {
					std::cout << " " << exportA.properties.at(j).floatValue << " ";
				}
				else if (exportA.properties.at(j).PropertyType == "FString") {
					std::cout << " " << exportA.properties.at(j).stringValue << " ";
				}
				std::cout << std::endl;
			}
		}
	}


	// Print thumbnail data
	for (const auto& thumbnail : data.thumbnails) {
		std::cout << "Thumbnail:" << std::endl;
		std::cout << "  Width: " << thumbnail.ImageWidth << std::endl;
		std::cout << "  Height: " << thumbnail.ImageHeight << std::endl;
		std::cout << "  Format: " << thumbnail.ImageFormat << std::endl;
		std::cout << "  Data Size: " << thumbnail.ImageSizeData << std::endl;
	}

	// Print asset registry data
	std::cout << "Asset Registry Data Size: " << data.assetRegistryData.size << std::endl;
	std::cout << "Dependency Data Offset: " << data.assetRegistryData.DependencyDataOffset << std::endl;

	for (const auto& entry : data.assetRegistryData.data) {
		std::cout << "Object Path: " << entry.ObjectPath << std::endl;
		std::cout << "Object Class Name: " << entry.ObjectClassName << std::endl;
		for (const auto& tag : entry.Tags) {
			std::cout << "  Tag Key: " << tag.Key << ", Tag Value: " << tag.Value << std::endl;
		}
	}
}



int main() {
	//    std::ifstream file("C:/Users/kapis/Downloads/Blueprint/BP_FrontEndPlayerController.uasset", std::ios::binary);
	std::ifstream file("C:/Users/kapis/Downloads/Blueprint/BP_SandWorldPlayerController.uasset", std::ios::binary);
	//    std::ifstream file("C:/Users/kapis/Downloads/Blueprint/BP_SaveGameState.uasset", std::ios::binary);

	if (!file) {
		std::cerr << "Failed to open file" << std::endl;
		return 1;
	}
	std::vector<uint8_t> bytes((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	Uasset uasset;
	if (!uasset.parse(bytes)) {
		std::cerr << "Failed to parse uasset file" << std::endl;
		return 1;
	}

	// Print parsed data
	printUassetData(uasset.data);

	// Convert to JSON
	json j = uasset.toJson();
	std::cout << j.dump(4) << std::endl;

	return 0;
}


