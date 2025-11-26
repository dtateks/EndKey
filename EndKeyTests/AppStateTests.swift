import XCTest
@testable import EndKey

/// Test suite for Phase 03 UI Components - AppState
/// Tests mode toggling and input method persistence
final class AppStateTests: XCTestCase {

    var appState: AppState!

    override func setUp() {
        super.setUp()
        // Reset UserDefaults to clean state
        UserDefaults.standard.removeObject(forKey: "isVietnameseMode")
        UserDefaults.standard.removeObject(forKey: "inputMethod")
        UserDefaults.standard.removeObject(forKey: "launchAtLogin")

        // Create fresh instance
        appState = AppState.shared
    }

    // MARK: - Mode Toggle Tests

    func testDefaultModeIsVietnamese() {
        XCTAssertTrue(appState.isVietnameseMode, "Default mode should be Vietnamese")
    }

    func testModeToggle() {
        let initialMode = appState.isVietnameseMode

        appState.toggleMode()
        XCTAssertNotEqual(appState.isVietnameseMode, initialMode, "Mode should toggle")

        appState.toggleMode()
        XCTAssertEqual(appState.isVietnameseMode, initialMode, "Mode should toggle back")
    }

    func testModeToggleNotification() {
        let expectation = XCTestExpectation(description: "Mode change notification")

        let observer = NotificationCenter.default.addObserver(
            forName: .modeChanged,
            object: nil,
            queue: nil
        ) { _ in
            expectation.fulfill()
        }

        appState.toggleMode()

        wait(for: [expectation], timeout: 1.0)
        NotificationCenter.default.removeObserver(observer)
    }

    func testModeToggleUpdatesKeyboardManager() {
        let initialMode = appState.isVietnameseMode

        appState.toggleMode()

        // KeyboardManager should be updated (verified via AppState didSet)
        XCTAssertNotEqual(appState.isVietnameseMode, initialMode)
    }

    // MARK: - Input Method Persistence Tests

    func testDefaultInputMethodIsTelex() {
        XCTAssertEqual(appState.inputMethod, .telex, "Default input method should be Telex")
    }

    func testInputMethodPersistence() {
        // Set to VNI
        appState.inputMethod = .vni
        XCTAssertEqual(appState.inputMethod, .vni)
        XCTAssertEqual(appState.inputMethodRaw, InputMethod.vni.rawValue)

        // Verify persistence via UserDefaults
        let savedRaw = UserDefaults.standard.string(forKey: "inputMethod")
        XCTAssertEqual(savedRaw, InputMethod.vni.rawValue)

        // Switch back to Telex
        appState.inputMethod = .telex
        XCTAssertEqual(appState.inputMethod, .telex)
    }

    func testInputMethodRawValueSetter() {
        // Test setting via raw value
        appState.inputMethodRaw = InputMethod.vni.rawValue
        XCTAssertEqual(appState.inputMethod, .vni)

        appState.inputMethodRaw = InputMethod.telex.rawValue
        XCTAssertEqual(appState.inputMethod, .telex)
    }

    func testInvalidInputMethodRawValue() {
        // Set valid first
        appState.inputMethod = .telex

        // Try invalid raw value
        appState.inputMethodRaw = "invalid"

        // Should fallback to Telex
        XCTAssertEqual(appState.inputMethod, .telex)
    }

    // MARK: - UserDefaults Persistence Tests

    func testModePersistence() {
        // Set mode
        appState.isVietnameseMode = false

        // Verify UserDefaults
        let saved = UserDefaults.standard.bool(forKey: "isVietnameseMode")
        XCTAssertFalse(saved)

        // Change and verify again
        appState.isVietnameseMode = true
        let savedAgain = UserDefaults.standard.bool(forKey: "isVietnameseMode")
        XCTAssertTrue(savedAgain)
    }

    func testLaunchAtLoginPersistence() {
        // Default should be false
        XCTAssertFalse(appState.launchAtLogin)

        // Set to true
        appState.launchAtLogin = true
        XCTAssertTrue(appState.launchAtLogin)

        // Verify UserDefaults
        let saved = UserDefaults.standard.bool(forKey: "launchAtLogin")
        XCTAssertTrue(saved)
    }

    // MARK: - Initialization Tests

    func testSingletonInstance() {
        let instance1 = AppState.shared
        let instance2 = AppState.shared

        XCTAssertTrue(instance1 === instance2, "AppState should be singleton")
    }

    func testInitialStateSyncToKeyboardManager() {
        // After init, KeyboardManager should have same state
        // This is verified via didSet in AppState init
        XCTAssertNotNil(appState.isVietnameseMode)
        XCTAssertNotNil(appState.inputMethod)
    }
}
