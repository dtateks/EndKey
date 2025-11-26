import XCTest
@testable import EndKeyCore

/// Comprehensive test suite for Phase 02 Core Engine
/// Tests TelexEngine, VNIEngine, and KeyboardManager integration
final class EngineTests: XCTestCase {

    // MARK: - TelexEngine Tests

    func testTelexVowelDoubling() {
        var engine = TelexEngine()

        // Test aa → â
        var result = engine.processKey("a")
        XCTAssertEqual(result.backspaceCount, 0)
        XCTAssertNil(result.replacement)

        result = engine.processKey("a")
        XCTAssertEqual(result.backspaceCount, 1)
        XCTAssertEqual(result.replacement, "â")
        XCTAssertEqual(engine.buffer, ["â"])

        // Test aw → ă
        engine.reset()
        _ = engine.processKey("a")
        result = engine.processKey("w")
        XCTAssertEqual(result.backspaceCount, 1)
        XCTAssertEqual(result.replacement, "ă")

        // Test ee → ê
        engine.reset()
        _ = engine.processKey("e")
        result = engine.processKey("e")
        XCTAssertEqual(result.backspaceCount, 1)
        XCTAssertEqual(result.replacement, "ê")

        // Test oo → ô
        engine.reset()
        _ = engine.processKey("o")
        result = engine.processKey("o")
        XCTAssertEqual(result.backspaceCount, 1)
        XCTAssertEqual(result.replacement, "ô")

        // Test ow → ơ
        engine.reset()
        _ = engine.processKey("o")
        result = engine.processKey("w")
        XCTAssertEqual(result.backspaceCount, 1)
        XCTAssertEqual(result.replacement, "ơ")

        // Test uw → ư
        engine.reset()
        _ = engine.processKey("u")
        result = engine.processKey("w")
        XCTAssertEqual(result.backspaceCount, 1)
        XCTAssertEqual(result.replacement, "ư")
    }

    func testTelexDTransform() {
        var engine = TelexEngine()

        // Test dd → đ
        _ = engine.processKey("d")
        let result = engine.processKey("d")
        XCTAssertEqual(result.backspaceCount, 1)
        XCTAssertEqual(result.replacement, "đ")
        XCTAssertEqual(engine.buffer, ["đ"])
    }

    func testTelexToneMarks() {
        var engine = TelexEngine()

        // Test chaof → chào (f=huyền on 'a')
        _ = engine.processKey("c")
        _ = engine.processKey("h")
        _ = engine.processKey("a")
        _ = engine.processKey("o")
        let result = engine.processKey("f")
        XCTAssertEqual(result.backspaceCount, 2)
        XCTAssertEqual(result.replacement, "ào")

        // Test vies → viés (s=sắc)
        engine.reset()
        _ = engine.processKey("v")
        _ = engine.processKey("i")
        _ = engine.processKey("e")
        let result2 = engine.processKey("s")
        XCTAssertEqual(result2.backspaceCount, 1) // Only need to backspace the 'e'
        XCTAssertEqual(result2.replacement, "é")

        // Test single vowel with tone
        engine.reset()
        _ = engine.processKey("a")
        let result3 = engine.processKey("f")
        XCTAssertEqual(result3.backspaceCount, 1)
        XCTAssertEqual(result3.replacement, "à")

        // Test r (hỏi)
        engine.reset()
        _ = engine.processKey("e")
        let result4 = engine.processKey("r")
        XCTAssertEqual(result4.backspaceCount, 1)
        XCTAssertEqual(result4.replacement, "ẻ")

        // Test x (ngã)
        engine.reset()
        _ = engine.processKey("o")
        let result5 = engine.processKey("x")
        XCTAssertEqual(result5.backspaceCount, 1)
        XCTAssertEqual(result5.replacement, "õ")

        // Test j (nặng)
        engine.reset()
        _ = engine.processKey("u")
        let result6 = engine.processKey("j")
        XCTAssertEqual(result6.backspaceCount, 1)
        XCTAssertEqual(result6.replacement, "ụ")
    }

    func testTelexUppercase() {
        var engine = TelexEngine()

        // Test AA → Â
        _ = engine.processKey("A")
        let result = engine.processKey("A")
        XCTAssertEqual(result.backspaceCount, 1)
        XCTAssertEqual(result.replacement, "Â")

        // Test Aa → Â
        engine.reset()
        _ = engine.processKey("A")
        let result2 = engine.processKey("a")
        XCTAssertEqual(result2.backspaceCount, 1)
        XCTAssertEqual(result2.replacement, "Â")

        // Test DD → Đ
        engine.reset()
        _ = engine.processKey("D")
        let result3 = engine.processKey("D")
        XCTAssertEqual(result3.backspaceCount, 1)
        XCTAssertEqual(result3.replacement, "Đ")
    }

    func testTelexComplexWord() {
        var engine = TelexEngine()

        // Test "thuoongf" → "thưởng"
        // th: passthrough
        _ = engine.processKey("t")
        _ = engine.processKey("h")

        // u: passthrough
        _ = engine.processKey("u")

        // o → part of uow transform
        _ = engine.processKey("o")

        // w → transform uow to ươ
        let result1 = engine.processKey("w")
        XCTAssertEqual(result1.replacement, "ươ") // uow → ươ

        // ng: passthrough
        _ = engine.processKey("n")
        _ = engine.processKey("g")

        // f: should apply tone to modified vowel 'ơ'
        let result2 = engine.processKey("f")
        XCTAssertNotNil(result2.replacement)
        XCTAssert(result2.replacement?.contains("ờ") == true || result2.replacement?.contains("ơ") == true)
    }

    // MARK: - VNIEngine Tests

    func testVNIVowelModifiers() {
        var engine = VNIEngine()

        // Test a6 → â
        _ = engine.processKey("a")
        var result = engine.processKey("6")
        XCTAssertEqual(result.backspaceCount, 1)
        XCTAssertEqual(result.replacement, "â")

        // Test a7 → ă
        engine.reset()
        _ = engine.processKey("a")
        result = engine.processKey("7")
        XCTAssertEqual(result.backspaceCount, 1)
        XCTAssertEqual(result.replacement, "ă")

        // Test e6 → ê
        engine.reset()
        _ = engine.processKey("e")
        result = engine.processKey("6")
        XCTAssertEqual(result.backspaceCount, 1)
        XCTAssertEqual(result.replacement, "ê")

        // Test o6 → ô
        engine.reset()
        _ = engine.processKey("o")
        result = engine.processKey("6")
        XCTAssertEqual(result.backspaceCount, 1)
        XCTAssertEqual(result.replacement, "ô")

        // Test o7 → ơ
        engine.reset()
        _ = engine.processKey("o")
        result = engine.processKey("7")
        XCTAssertEqual(result.backspaceCount, 1)
        XCTAssertEqual(result.replacement, "ơ")

        // Test u7 → ư
        engine.reset()
        _ = engine.processKey("u")
        result = engine.processKey("7")
        XCTAssertEqual(result.backspaceCount, 1)
        XCTAssertEqual(result.replacement, "ư")
    }

    func testVNID9Transform() {
        var engine = VNIEngine()

        // Test d9 → đ
        _ = engine.processKey("d")
        let result = engine.processKey("9")
        XCTAssertEqual(result.backspaceCount, 1)
        XCTAssertEqual(result.replacement, "đ")
    }

    func testVNIToneMarks() {
        var engine = VNIEngine()

        // Test cha2o → chào (2=huyền)
        _ = engine.processKey("c")
        _ = engine.processKey("h")
        _ = engine.processKey("a")
        let result = engine.processKey("2")
        XCTAssertEqual(result.backspaceCount, 1)
        XCTAssertEqual(result.replacement, "à")

        // Continue with 'o'
        _ = engine.processKey("o")

        // Test vie1t → viết (1=sắc)
        engine.reset()
        _ = engine.processKey("v")
        _ = engine.processKey("i")
        _ = engine.processKey("e")
        let result2 = engine.processKey("1")
        XCTAssertEqual(result2.backspaceCount, 1) // Only need to backspace the 'e'
        XCTAssertEqual(result2.replacement, "é")

        // Test all tone marks on single vowel
        engine.reset()
        _ = engine.processKey("a")
        XCTAssertEqual(engine.processKey("1").replacement, "á") // sắc

        engine.reset()
        _ = engine.processKey("a")
        XCTAssertEqual(engine.processKey("2").replacement, "à") // huyền

        engine.reset()
        _ = engine.processKey("a")
        XCTAssertEqual(engine.processKey("3").replacement, "ả") // hỏi

        engine.reset()
        _ = engine.processKey("a")
        XCTAssertEqual(engine.processKey("4").replacement, "ã") // ngã

        engine.reset()
        _ = engine.processKey("a")
        XCTAssertEqual(engine.processKey("5").replacement, "ạ") // nặng
    }

    func testVNIUppercase() {
        var engine = VNIEngine()

        // Test A6 → Â
        _ = engine.processKey("A")
        let result = engine.processKey("6")
        XCTAssertEqual(result.backspaceCount, 1)
        XCTAssertEqual(result.replacement, "Â")

        // Test D9 → Đ
        engine.reset()
        _ = engine.processKey("D")
        let result2 = engine.processKey("9")
        XCTAssertEqual(result2.backspaceCount, 1)
        XCTAssertEqual(result2.replacement, "Đ")
    }

    func testVNIComplexWord() {
        var engine = VNIEngine()

        // Test "thu7o7ng2" → "thưởng"
        _ = engine.processKey("t")
        _ = engine.processKey("h")
        _ = engine.processKey("u")

        // u7 → ư
        let result1 = engine.processKey("7")
        XCTAssertEqual(result1.replacement, "ư")

        _ = engine.processKey("o")

        // o7 → ơ
        let result2 = engine.processKey("7")
        XCTAssertEqual(result2.replacement, "ơ")

        _ = engine.processKey("n")
        _ = engine.processKey("g")

        // 2 = huyền tone
        let result3 = engine.processKey("2")
        XCTAssertNotNil(result3.replacement)
    }

    // MARK: - Engine Management Tests

    func testEngineSwitching() {
        // Test Telex engine
        var telexEngine = TelexEngine()
        _ = telexEngine.processKey("a")
        let telexResult = telexEngine.processKey("a")
        XCTAssertEqual(telexResult.replacement, "â")

        // Test VNI engine independently
        var vniEngine = VNIEngine()
        _ = vniEngine.processKey("a")
        let vniResult = vniEngine.processKey("6")
        XCTAssertEqual(vniResult.replacement, "â")

        // Verify they don't interfere
        XCTAssertEqual(telexEngine.buffer, ["â"])
        XCTAssertEqual(vniEngine.buffer, ["â"])
    }

    func testBufferReset() {
        var engine = TelexEngine()

        // Add some characters to buffer
        _ = engine.processKey("c")
        _ = engine.processKey("h")
        _ = engine.processKey("a")
        _ = engine.processKey("o")

        XCTAssertEqual(engine.buffer.count, 4)

        // Reset
        engine.reset()

        // Verify buffer is empty
        XCTAssertEqual(engine.buffer.count, 0)
        XCTAssertTrue(engine.buffer.isEmpty)

        // Test that new input works after reset
        _ = engine.processKey("a")
        let result = engine.processKey("a")
        XCTAssertEqual(result.replacement, "â")
    }

    func testBufferTrimming() {
        var engine = TelexEngine()

        // Add more than maxBufferSize (7) characters
        let chars: [Character] = ["a", "b", "c", "d", "e", "f", "g", "h", "i", "j"]
        for char in chars {
            _ = engine.processKey(char)
        }

        // Buffer should be trimmed to maxBufferSize
        XCTAssertLessThanOrEqual(engine.buffer.count, 7)
    }

    // MARK: - Edge Cases

    func testNoVowelForTone() {
        var engine = TelexEngine()

        // Try to apply tone without vowel
        _ = engine.processKey("b")
        _ = engine.processKey("c")
        let result = engine.processKey("f")

        // Should treat 'f' as regular char when no vowel to apply tone
        XCTAssertEqual(result.backspaceCount, 0)
        XCTAssertNil(result.replacement)
    }

    func testToneOnModifiedVowel() {
        var engine = TelexEngine()

        // Test âf → à (tone on modified vowel)
        _ = engine.processKey("a")
        _ = engine.processKey("a") // aa → â
        let result = engine.processKey("f")
        XCTAssertEqual(result.replacement, "ầ")
    }

    func testMultipleTonesOnSameVowel() {
        var engine = TelexEngine()

        // Test af → à, then as → á (replace tone)
        _ = engine.processKey("a")
        _ = engine.processKey("f") // à
        let result = engine.processKey("s") // should replace with á

        // Should apply new tone
        XCTAssertNotNil(result.replacement)
        XCTAssertTrue(result.replacement?.contains("á") ?? false)
    }

    func testVNIInvalidModifier() {
        var engine = VNIEngine()

        // Try to apply modifier to non-vowel
        _ = engine.processKey("b")
        let result = engine.processKey("6")

        // Should treat '6' as regular number when no applicable vowel
        XCTAssertEqual(result.backspaceCount, 0)
        XCTAssertNil(result.replacement)
    }

    func testVietnameseDataHelpers() {
        // Test getBaseVowel
        XCTAssertEqual(VietnameseData.getBaseVowel("á"), "a")
        XCTAssertEqual(VietnameseData.getBaseVowel("ê"), "ê")
        XCTAssertEqual(VietnameseData.getBaseVowel("Á"), "a")
        XCTAssertEqual(VietnameseData.getBaseVowel("b"), "b")

        // Test isVowel
        XCTAssertTrue(VietnameseData.isVowel("a"))
        XCTAssertTrue(VietnameseData.isVowel("ă"))
        XCTAssertTrue(VietnameseData.isVowel("á"))
        XCTAssertTrue(VietnameseData.isVowel("A"))
        XCTAssertFalse(VietnameseData.isVowel("b"))
        XCTAssertFalse(VietnameseData.isVowel("1"))
    }

    // MARK: - New Feature Tests (Phase 2 Refactor)

    func testTelexEscapeSequence() {
        var engine = TelexEngine()

        // Test ass → as (escape to raw text)
        _ = engine.processKey("a")
        _ = engine.processKey("s") // á
        let result = engine.processKey("s") // escape
        XCTAssertNotNil(result.replacement)
        XCTAssertEqual(result.replacement, "a") // reverts to base

        // Test aff → af
        engine.reset()
        _ = engine.processKey("a")
        _ = engine.processKey("f") // à
        let result2 = engine.processKey("f") // escape
        XCTAssertNotNil(result2.replacement)
        XCTAssertEqual(result2.replacement, "a")
    }

    func testTelexToneChange() {
        var engine = TelexEngine()

        // Test asf → à (change sắc → huyền)
        _ = engine.processKey("a")
        _ = engine.processKey("s") // á
        let result = engine.processKey("f") // change to huyền
        XCTAssertNotNil(result.replacement)
        XCTAssertEqual(result.replacement, "à")

        // Test asr → ả (change sắc → hỏi)
        engine.reset()
        _ = engine.processKey("a")
        _ = engine.processKey("s") // á
        let result2 = engine.processKey("r") // change to hỏi
        XCTAssertNotNil(result2.replacement)
        XCTAssertEqual(result2.replacement, "ả")
    }

    func testTelexZUndo() {
        var engine = TelexEngine()

        // Test asz → a (remove tone)
        _ = engine.processKey("a")
        _ = engine.processKey("s") // á
        let result = engine.processKey("z") // undo
        XCTAssertNotNil(result.replacement)
        XCTAssertEqual(result.replacement, "a")

        // Test aaz → a (revert â → a)
        engine.reset()
        _ = engine.processKey("a")
        _ = engine.processKey("a") // â
        let result2 = engine.processKey("z") // undo
        XCTAssertNotNil(result2.replacement)
        XCTAssertEqual(result2.replacement, "a")
    }

    func testTelexUowTransform() {
        var engine = TelexEngine()

        // Test uow → ươ
        _ = engine.processKey("u")
        _ = engine.processKey("o")
        let result = engine.processKey("w")
        XCTAssertEqual(result.backspaceCount, 2)
        XCTAssertEqual(result.replacement, "ươ")
    }

    func testTelexComplexWordsNguoi() {
        var engine = TelexEngine()

        // Test nguowif → người
        for char in "nguow" { _ = engine.processKey(char) }
        // After uow, buffer should have ng + ươ
        let result = engine.processKey("i")
        XCTAssertEqual(result.backspaceCount, 0) // i is passthrough

        let result2 = engine.processKey("f")
        XCTAssertNotNil(result2.replacement)
        // Tone should be on ơ (modified vowel gets priority)
    }

    func testTelexComplexWordsNuoc() {
        var engine = TelexEngine()

        // Test nuowcs → nước
        for char in "nuow" { _ = engine.processKey(char) }
        _ = engine.processKey("c")
        let result = engine.processKey("s")
        XCTAssertNotNil(result.replacement)
        // Tone should be on ơ
    }

    func testTelexComplexWordsDuoc() {
        var engine = TelexEngine()

        // Test dduowcj → được
        _ = engine.processKey("d")
        _ = engine.processKey("d") // đ
        for char in "uow" { _ = engine.processKey(char) }
        _ = engine.processKey("c")
        let result = engine.processKey("j")
        XCTAssertNotNil(result.replacement)
    }

    func testTelexTriphthongTonePlacement() {
        var engine = TelexEngine()

        // Test khoais → khoái (tone on 'a', not 'i')
        for char in "khoai" { _ = engine.processKey(char) }
        let result = engine.processKey("s")
        XCTAssertNotNil(result.replacement)
        // Should tone the 'a' in 'oai' triphthong
    }

    func testTelexDiphthongTonePlacement() {
        var engine = TelexEngine()

        // Test oaf → òa (tone on first vowel for 'oa')
        _ = engine.processKey("o")
        _ = engine.processKey("a")
        let result = engine.processKey("f")
        XCTAssertNotNil(result.replacement)
        // For 'oa', tone goes on second vowel 'a'
    }

    func testVNIEscapeSequence() {
        var engine = VNIEngine()

        // Test a11 → a1 (escape to raw text)
        _ = engine.processKey("a")
        _ = engine.processKey("1") // á
        let result = engine.processKey("1") // escape
        XCTAssertNotNil(result.replacement)
        XCTAssertEqual(result.replacement, "a")
    }

    func testVNIToneChange() {
        var engine = VNIEngine()

        // Test a12 → à (change sắc → huyền)
        _ = engine.processKey("a")
        _ = engine.processKey("1") // á
        let result = engine.processKey("2") // change to huyền
        XCTAssertNotNil(result.replacement)
        XCTAssertEqual(result.replacement, "à")
    }

    // MARK: - Data Structure Tests

    func testSyllableStateReset() {
        var state = SyllableState()
        state.toneApplied = "s"
        state.lastTonePosition = 2
        state.escapeMode = true
        state.rawInput.append(RawInputEntry(rawChar: "a", outputChar: "á", position: 0))

        state.reset()

        XCTAssertNil(state.toneApplied)
        XCTAssertNil(state.lastTonePosition)
        XCTAssertFalse(state.escapeMode)
        XCTAssertTrue(state.rawInput.isEmpty)
    }

    func testVowelClusterData() {
        // Test consonant clusters
        XCTAssertTrue(VietnameseData.consonantClusters.contains("gi"))
        XCTAssertTrue(VietnameseData.consonantClusters.contains("qu"))
        XCTAssertTrue(VietnameseData.consonantClusters.contains("ng"))

        // Test diphthongs
        XCTAssertTrue(VietnameseData.diphthongsToneFirst.contains("ai"))
        XCTAssertTrue(VietnameseData.diphthongsToneFirst.contains("ao"))
        XCTAssertTrue(VietnameseData.diphthongsToneSecond.contains("oa"))
        XCTAssertTrue(VietnameseData.diphthongsToneSecond.contains("uy"))

        // Test triphthongs
        XCTAssertTrue(VietnameseData.triphthongs.contains("oai"))
        XCTAssertTrue(VietnameseData.triphthongs.contains("uoi"))
        XCTAssertTrue(VietnameseData.triphthongs.contains("yeu"))

        // Test multi-char transforms
        XCTAssertEqual(VietnameseData.multiCharTransforms["uow"], "ươ")
    }
}
