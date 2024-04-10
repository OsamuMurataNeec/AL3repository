#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {
	// 自キャラの解放
	delete player_;

	//ブロック解放
	for (WorldTransform* worldTransformBlock : worldTransformBlocks_) {
		delete worldTransformBlock;
	}
	worldTransformBlocks_.clear();
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("mario.jpg");

	// 3Dモデルの生成
	model_ = Model::Create();
	modelBlock_ = Model::Create();
	// 自キャラの生成
	player_ = new Player();

	// ビュープロジェクションの初期化
	viewProjection_.Initialize();

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// 自キャラの初期化
	player_->Initialize(model_, textureHandle_, &viewProjection_);

	// キューブの生成
	// 要素数
	const uint32_t kNumBlockHorizontal = 20;
	// ブロック1個分の横幅
	const float kBlockWidth = 2.0f;
	// 要素数を変更
	worldTransformBlocks_.resize(kNumBlockHorizontal);
	for (uint32_t i = 0; i < kNumBlockHorizontal; ++i){
		worldTransformBlocks_[i] = new WorldTransform();
		worldTransformBlocks_[i]->Initialize();
		worldTransformBlocks_[i]->translation_.x = kBlockWidth * i;
		worldTransformBlocks_[i]->translation_.y = 0.0f;
	}
}

void GameScene::Update() {

	// 自キャラの更新
	player_->Update();

	for (uint32_t i = 0; i < 20; ++i) {
		worldTransformBlocks_[i]->matWorld_ = MakeAffineMatrix(
		    worldTransformBlocks_[i]->scale_, 
			worldTransformBlocks_[i]->rotation_,
		    worldTransformBlocks_[i]->translation_);

		// 定数バッファに転送する
		worldTransformBlocks_[i]->TransferMatrix();
	}
	/*
	// ブロックの更新
	for (WorldTransform* worldTransformBlock : worldTransformBlocks_) {

		// 行列更新
		worldTransformBlock->matWorld_ = MakeAffineMatrix(
		    worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

		// 定数バッファに転送する
		worldTransformBlock->TransferMatrix();
	}
	*/
}


void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	// 自キャラの描画
//	player_->Draw();

	// ブロックの描画
//	for (WorldTransform* worldTransformBlock : worldTransformBlocks_) {
//		modelBlock_->Draw(*worldTransformBlock, viewProjection_, textureHandle_);
//	}
	for (uint32_t i = 0; i < 20; i++) {
		modelBlock_->Draw(*worldTransformBlocks_[i], viewProjection_, textureHandle_);
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
