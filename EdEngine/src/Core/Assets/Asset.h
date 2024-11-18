#pragma once

#include "Core/Assets/ImportParameters/AssetImportParameters.h"

enum class AssetType : uint8_t
{
	None,
	Texture2D,
	CubeTexture,
	Texture2DArray,
	Material,
	StaticMesh,
	StaticSubmesh
};

ED_CLASS(Asset) : public GameObject
{
	ED_CLASS_BODY(Asset, GameObject)
public:
	using ImportParametersClass = AssetImportParameters;

	Asset(const std::string& name = "Empty");

	uint64_t GetId() const;
	virtual AssetType GetType() const;

	virtual bool HasData() const;
	virtual void ClaimData();
	virtual void UnclaimData();

    void MarkDirty();
    bool IsDirty() const;

	virtual void SetShouldLoadData(bool status);
	bool ShouldHaveData() const;

	void SetImportParameters(std::shared_ptr<AssetImportParameters> parameters);
	std::shared_ptr<AssetImportParameters> GetImportParameters() const;

	virtual void ResetState();

	template<typename T>
	std::shared_ptr<T> GetImportParameters() const
	{
		return std::static_pointer_cast<T>(GetImportParameters());
	}

	virtual void Serialize(Archive& archive) override;
	virtual void SerializeData(Archive& archive);
	virtual void FreeData();

	virtual ~Asset() = default;
protected:
	uint64_t m_Id;

	bool m_bHasData = false;

	int32_t m_DataClaims = 0;
	bool m_bIsDirty = false;
	bool m_bShouldHaveData = false;

	std::shared_ptr<AssetImportParameters> m_ImportParameters;
};