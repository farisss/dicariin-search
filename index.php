<?php
	
/*
 * Dicariin Search Engine Project
 * Copyright (C) Faris Khowarizmi Tarmizi
 * eMail: thekill96[at]gmail.com
 *
 * Main index page section
 */
	
	require_once "query-utils.php";
	isset($_GET['max'])? $maxTerm = $_GET['max']: $maxTerm = 20;
	// Ada kueri yang masuk?
	if (isset($_GET['q'])){
		$query = $_GET['q'];
		if ($query != ""){
			// Lakukan pencarian!
			$qdata = doSearchQuery($query, $maxTerm);
			$qcount = $qdata['resultCount'];
		}
		// Pastikan query tidak merusak dokumen html
		$query = htmlentities($query);
	}
	else{
		$query = "";
	}
	
?>
<!DOCTYPE HTML>
<html>
<head>
<?php
	include "headdoc.php";
?>
</head>

<body>

<?php
	
	// Tampilkan halaman list pencarian jika ada kueri
	// Jika tidak, gunakan halaman awal
	if ($query != "")
		include "searchlst.php";
	else
		include "intro.php";
	
?>

<!-- Footer bawah -->
<footer class="footer">
  <div class="container-fluid">
    <span class="text-muted"><p class="text-center"><a href="http://cs.unsyiah.ac.id/~frmizi" target="_blank">Copyright &copy; Thiekus 2017</a><?php if (isset($qdata)){printf(" - Query time: %g mseconds", $qdata['queryTime']);} ?></p></span>
  </div>
</footer>

</body>
</html>