// swift-tools-version:5.9
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
    name: "EndKey",
    platforms: [
        .macOS(.v11)
    ],
    products: [
        .library(
            name: "EndKeyCore",
            targets: ["EndKeyCore"]
        ),
    ],
    targets: [
        // Core library (contains engines and data)
        .target(
            name: "EndKeyCore",
            path: "EndKey",
            sources: [
                "Core/VietnameseData.swift",
                "Core/InputEngine.swift",
                "Core/TelexEngine.swift",
                "Core/VNIEngine.swift",
                "Core/KeyboardManager.swift",
                "Models/AppState.swift",
                "Utils/PermissionHelper.swift"
            ]
        ),

        // Test target
        .testTarget(
            name: "EndKeyTests",
            dependencies: ["EndKeyCore"],
            path: "EndKeyTests"
        ),
    ]
)
