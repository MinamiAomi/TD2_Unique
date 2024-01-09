#include "Sprite.h"

#include "Core/SamplerManager.h"

void Texture::Load(const std::filesystem::path& path) {
    resource_.CreateFromWICFile(path.wstring());
}

const DescriptorHandle& Texture::GetSampler() const {
	switch (interpolation_) {
	case Texture::Interpolation::Linear:
		switch (extension_) {
		case Texture::Extension::Wrap:
			return SamplerManager::LinearWrap;
		case Texture::Extension::Clamp:
			return SamplerManager::LinearClamp;
		}
		break;
	case Texture::Interpolation::Point:
		switch (extension_) {
		case Texture::Extension::Wrap:
			return SamplerManager::PointWrap;
		case Texture::Extension::Clamp:
			return SamplerManager::PointClamp;
		}
		break;
	}
    return SamplerManager::AnisotropicWrap;
}

std::list<Sprite*> Sprite::instanceList_;

Sprite::Sprite() {
	instanceList_.emplace_back(this);
}

Sprite::~Sprite() {
	instanceList_.remove(this);
}
