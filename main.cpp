#include <Novice.h>
#include "GeometryUtility.h"
#include <imgui.h>

const char kWindowTitle[] = "学籍番号";

GeometryUtility geometryUtility;

Vector3 Lerp(const Vector3& a, const Vector3& b, float t)
{
	Vector3 result;
	result.x = t * a.x + (1.0f - t) * b.x;
	result.y = t * a.y + (1.0f - t) * b.y;
	result.z = t * a.z + (1.0f - t) * b.z;
	return result;
}
void DrawBezier(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2,
	const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewMatrix, uint32_t color) {

	Sphere sphere[3] = {
		{controlPoint0,0.01f},
		{controlPoint1,0.01f},
		{controlPoint2,0.01f},
	};
	for (int i = 0; i < 3; i++)
	{
		geometryUtility.DrawSphere(sphere[i], viewProjectionMatrix, viewMatrix, 0x000000FF);
	}
	const float segments = 100.0f;
	for (int i = 0; i < segments; ++i) {
		float t0 = float(i / segments);
		float t1 = float((i + 1) / segments);

		Vector3 p0 = Lerp(controlPoint0, controlPoint1, t0);
		Vector3 p1 = Lerp(controlPoint1, controlPoint2, t0);
		Vector3 point0 = Lerp(p0, p1, t0);

		p0 = Lerp(controlPoint0, controlPoint1, t1);
		p1 = Lerp(controlPoint1, controlPoint2, t1);
		Vector3 point1 = Lerp(p0, p1, t1);

		point0 = geometryUtility.Transform(geometryUtility.Transform(point0, viewProjectionMatrix), viewMatrix);
		point1 = geometryUtility.Transform(geometryUtility.Transform(point1, viewProjectionMatrix), viewMatrix);

		Novice::DrawLine(int(point0.x), int(point0.y), int(point1.x), int(point1.y), color);
	}

}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	Vector3 rotate{ 0.0f,0.0f,0.0f };

	Vector3 translate{ 0.0f,0.0f,0.0f };


	int kWindowWidth = 1280;
	int kWindowHeight = 720;

	float radius = 10.0f;
	float theta = -3.46f;
	float phi = 1.5f;
	

	Vector3 controlPoints[3] = {
		{-0.8f,0.58f,1.0f},
		{1.76f,1.0f,-0.3f},
		{0.94f,-0.7f,2.3f},
	};
	



	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		Vector3 cameraPosition = geometryUtility.SphericalToCartesian(radius, theta, phi);
		Vector3 cameraTarget = { 0.0f, 0.0f, 0.0f };
		Vector3 cameraUp = { 0.0f, 1.0f, 0.0f };

		Matrix4x4 viewMatrix = geometryUtility.MakeLookAtMatrix4x4(cameraPosition, cameraTarget, cameraUp);
		Matrix4x4 worldMatrix = geometryUtility.MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotate, translate);
		Matrix4x4 projectionMatrix = geometryUtility.MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = geometryUtility.Multiply(worldMatrix, geometryUtility.Multiply(viewMatrix, projectionMatrix));
		Matrix4x4 viewProjectionMatrix = geometryUtility.Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = geometryUtility.MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);


		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		geometryUtility.DrawGrid(viewProjectionMatrix, viewportMatrix);
		DrawBezier(controlPoints[0], controlPoints[1], controlPoints[2], viewProjectionMatrix, viewportMatrix, 0x0000FFFF);

		ImGui::Begin("Window");
		ImGui::DragFloat("theta", &theta, 0.01f);
		ImGui::DragFloat("phi", &phi, 0.01f);
		ImGui::DragFloat3("controlPoints0", &controlPoints[0].x, 0.01f);
		ImGui::DragFloat3("controlPoints1", &controlPoints[1].x, 0.01f);
		ImGui::DragFloat3("controlPoints2", &controlPoints[2].x, 0.01f);
		ImGui::End();
		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
