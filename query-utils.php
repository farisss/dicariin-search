<?php

/*
 * Dicariin Search Engine Project
 * Copyright (C) Faris Khowarizmi Tarmizi
 * eMail: thekill96[at]gmail.com
 *
 * Query utily functions
 */

/* Melakukan pencarian */
function doSearchQuery($query, $num = 0){
	// Default max hash ke 15
	if ($num == 0)
		$num = 15;
	$num = addslashes($num);
	// Escaping pada berberapa karakter, dan jalankan kueri
	$query = addslashes(strtolower($query));
	// Windows:   "queryjson.exe \"$query\" $num"
	// Unix-like: "./queryjson \"$query\" $num"
	$data = exec("./queryjson \"$query\" $num");
	return json_decode($data, true);
}

/* Parsing dokumen yang terindeks */
function parseDocument($fileDoc){
	// Ambil data dari file dokumen
	$xmlFile = fopen($fileDoc, "r");
	$xmlData = fread($xmlFile, fileSize($fileDoc));
	fclose($xmlFile);
	
	// Workaround perbaiki dokumen xml untuk mengatasi dokumen tidak bisa diparsing
	// 1st: encode ke utf8, hilangkan semua karakter asing, dan ubah karakter xml
	//      ke subtitusinya
	$xmlData = htmlentities(preg_replace('/[\x00-\x1F\x7F]/u', '', utf8_encode($xmlData)), ENT_COMPAT || ENT_HTML401);
	// 2nd: ubah kembali tag xml yg sudah disubtitusikan kembali ke tag xml
	$xmlData = str_ireplace("&lt;title&gt;", "<title>", $xmlData);
	$xmlData = str_ireplace("&lt;/title&gt;", "</title>", $xmlData);
	$xmlData = str_ireplace("&lt;content&gt;", "<content>", $xmlData);
	$xmlData = str_ireplace("&lt;/content&gt;", "</content>", $xmlData);
	// 3rd: subtitusikan karakter dan '&' ke &amp;, mencegah parsing error jika ada
	//      karakter & dalam konten/title
	$xmlData = str_replace("&", "&amp;", $xmlData);
	// final: kempit isi xml diantara tag utama <document>
	$xmlData = "<document>" . $xmlData . "</document>";
	
	// Load data xml yang sudah diperbaiki tadi
	$xmlDoc = new DOMDocument;
	$xmlDoc->loadXML($xmlData);
	if (!$xmlDoc){
		echo 'Error while parsing the document $fileDoc';
		return;
	}
	// Pasring xml ke simplexml
	$parsedXml = simplexml_import_dom($xmlDoc);
	return $parsedXml;
}

/* Menebalkan caption hasil pencarian berdasarkan kueri */
function highlightCaption($caption, $query){
	// Belah query menjadi tiap term, lalu replace dengan highlight
	$qrArray = explode(" ", strtolower($query));
	$capt = $caption;
	foreach ($qrArray as $qterm){
		$capt = str_ireplace($qterm, "<strong>$qterm</strong>", $capt);
	}
	return $capt;
}

/* Membuat excerpt (preview dari isi dokumen) sesuai dengan kueri */
function getExcerpt($content, $query, $maxlen){
	// Belah query menjadi tiap term, lalu cari term dengan posisi terdekat
	$qrArray = explode(" ", strtolower($query));
	$csize = strlen($content);
	$spos = $csize;
	// Cari posisi kata untuk tiap term
	foreach ($qrArray as $qterm){
		$qpos = stripos($content, $qterm);
		if (($qpos < $spos) && ($qpos > 0)){
			$spos = $qpos;
		}
	}
	// Posisi diujung tanduk, mulai dari awal
	if ($spos == $csize){
		$spos = 0;
	}
	// Ambil bagian dari string sebanyak maksimum karakter excerpt
	$excerpt = substr($content, $spos, $maxlen);
	$sublen = strlen($excerpt);
	$maxarr = $sublen < $maxlen ? $sublen: $maxlen;
	$epos = $maxarr;
	$earray = str_split($excerpt);
	// Cari karakter tanda baca terdekat, menghindari excerpt dengan teks yg nanggung
	while ($epos > 0){
		$epos--;
		$kar = $earray[$epos];
		if (($kar === ' ') || ($kar === ',') || ($kar === '.') || ($kar === '!') || ($kar === '?')){
			break;
		}
	}
	if ($epos == 0){
		$epos = $maxarr;
	}
	$excerpt = substr($excerpt, 0, $epos);
	// Tebalkan kata term dari kueri pencarian
	foreach ($qrArray as $qterm){
		$excerpt = str_ireplace($qterm, "<strong>$qterm</strong>", $excerpt);
	}
	if ($earray[$epos] === ' '){
		$excerpt = $excerpt . "...";
	}
	return $excerpt;
}

?>