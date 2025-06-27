#pragma once

class TextureData
{
public:
	TextureData(TextureType type);
	TextureData(TextureType type, u8* data, u32 size, bool bTakeOwnership);

	TextureData& operator=(const TextureData& data);
	TextureData& operator=(TextureData&& data);

	virtual void Serialize(Archive<>& archive);

	void SetData(u8* data, u32 size, bool bTakeOwnership);
	
	u8* GetData() const { return m_Data; }
	u32 GetDataSize() const { return m_DataSize; }
	TextureType GetTextureType() const { return m_Type; }

	virtual ~TextureData();
protected:
	void FreeData();
private:
	u32 m_DataSize = 0;
	bool m_bDataOwner = false;
	u8* m_Data = nullptr;
	TextureType m_Type; // TODO: Serialize
};

class Texture2DData : public TextureData
{
public:
	Texture2DData();
	Texture2DData(i32 width, i32 height);
	Texture2DData(i32 width, i32 height, u8* data, u32 size, bool bTakeOwnership);

	Texture2DData& operator=(const Texture2DData& data);
	Texture2DData& operator=(Texture2DData&& data);

	void SetWidth(u32 width);
	u32 GetWidth() const;

	void SetHeight(u32 height);
	u32 GetHeight() const;

	void SetSize(glm::u32vec2 size);
	glm::u32vec2 GetSize() const;

	virtual void Serialize(Archive<>& archive) override;
private:
	u32 m_Width = 0;
	u32 m_Height = 0;
};

class Texture2DArrayData : public TextureData
{
public:
	Texture2DArrayData();
	Texture2DArrayData(i32 width, i32 height, i32 depth);
	Texture2DArrayData(i32 width, i32 height, i32 depth, u8* data, u32 size, bool bTakeOwnership);

	Texture2DArrayData& operator=(const Texture2DArrayData& data);
	Texture2DArrayData& operator=(Texture2DArrayData&& data);

	void SetWidth(u32 width);
	u32 GetWidth() const;

	void SetHeight(u32 height);
	u32 GetHeight() const;

	void SetDepth(u32 depth);
	u32 GetDepth() const;

	void SetSize(glm::u32vec3 size);
	glm::u32vec3 GetSize() const;

    virtual void Serialize(Archive<>& archive) override;
private:
	u32 m_Width = 0;
	u32 m_Height = 0;
	u32 m_Depth = 0;
};