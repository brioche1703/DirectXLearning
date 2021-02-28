#pragma once

#include "IndexedTriangleList.h"
#include "MathsUtils.h"

class Cone {
public:
	template<class V>
	static IndexedTriangleList<V> MakeTesselated(int longDiv) {
		namespace dx = DirectX;
		assert(longDiv > 2);

		const auto base = dx::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);
		const float longitudeAngle = 2.0f * PI / longDiv;

		std::vector<V> vertices;
		for (int iLong = 0; iLong < longDiv; iLong++) {
			vertices.emplace_back();
			auto v = dx::XMVector3Transform(
				base,
				dx::XMMatrixRotationZ(longitudeAngle * iLong)
			);
			dx::XMStoreFloat3(&vertices.back().pos, v);
		}

		// center vertice of base
		vertices.emplace_back();
		vertices.back().pos = { 0.0f, 0.0f, -1.0f };
		const auto iCenter = (unsigned short)(vertices.size() - 1);

		// Tip of the cone
		vertices.emplace_back();
		vertices.back().pos = { 0.0f, 0.0f,  1.0f };
		const auto iTip = (unsigned short)(vertices.size() - 1);

		std::vector<unsigned short> indices;
		// base
		for (unsigned short iLong = 0; iLong < longDiv; iLong++) {
			indices.push_back(iCenter);
			indices.push_back((iLong + 1) % longDiv);
			indices.push_back(iLong);
		}
		// cone
		for (unsigned short iLong = 0; iLong < longDiv; iLong++) {
			indices.push_back(iLong);
			indices.push_back((iLong + 1) % longDiv);
			indices.push_back(iTip);
		}

		return { std::move(vertices), std::move(indices) };
	}

	template<class V>
	static IndexedTriangleList<V> Make() {
		return MakeTesselated<V>(24);
	}
};
