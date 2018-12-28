<!-- Begin top navbar -->
<nav class="navbar navbar-default">
   <div class="navbar-header">
      <a class="navbar-brand font-rabbid2" href="index.php">Dicariin Search</a>
   </div>
   <form name="formcari" class="navbar-form navbar-left" method="get" action="">
      <div class="container-fluid form-group">
         <input type="text" class="form-control" name="q" value="<?php echo $query; ?>" placeholder="Cari disini..." style="width:250px;">
         <select name="max" class="form-control">
            <option value="10">Top 10</option>
            <option value="20" selected>Top 20</option>
            <option value="25">Top 25</option>
            <option value="50">Top 50</option>
            <option value="75">Top 75</option>
            <option value="100">Top 100</option>
         </select>
         <button class="btn btn-default" type="submit">
		   <i class="glyphicon glyphicon-search"></i>
		 </button>
      </div>
   </form>
</nav>

<!-- List pencarian -->
<div class="container">
<?php
	
	if ($query != ""){
		
		//echo "<pre>"; var_dump($qdata); echo "</pre>";
		if ($qcount > 0){
			
			// Heading hasil pencarian dan info banyak data yg didapatkan
			echo "<h2 class=\"font-bebas\">Hasil Pencarian</h2>";
			printf("\n<div class=\"well\"><span class=\"info-icon\">Menampilkan %d hasil pencarian '<u>%s</u>' untuk top %d</span></div>\n", $qcount, $query, $qdata['heapSize']);
			
			// Loop hasil pencarian
			echo "<ul class=\"list-group\">";
			printf("\n");
			for ($x=0; $x<$qcount; $x++){
				// Ambil data dari array of results, dan set paramter2 penting lainnya
				$entry = $qdata['results'][$x];
				$docPath = $entry['docPath'];
				$docData = parseDocument($docPath);
				$docTitle = highlightCaption($docData->title, $query);
				$docContent = getExcerpt($docData->content, $query, 300);
				$docLink = "http://" . $_SERVER['SERVER_NAME'] . dirname($_SERVER['PHP_SELF']) . "/" . $docPath;
				
				echo "<li class=\"list-group-item\">";
				echo "<p class=\"entry-title\"><a href=\"$docLink\">$docTitle</a></p>";
				echo "<p>$docContent</p>";
				//printf ("<pre>Rank value: %g\tDocument ID: %d\t\tLocation: %s</pre>", $entry['rankValue'], $entry['docNo'], $docLink);
				echo "<div class=\"panel panel-default\"><div class=\"panel-body small\">";
				printf ("<div>Rank value: %g</div><div>Document ID: %d</div><div>Location: %s</div>", $entry['rankValue'], $entry['docNo'], $docLink);
				echo "</div></div>";
				echo "</li>";
				printf("\n");
			}
			echo "</ul>";
			
			//printf("<p>Query time: %g mseconds</p>\n", $qdata['queryTime']);
			
		}
		else {
			
			// Tidak ada data yang ditemukan!
			echo "<h2 class=\"font-bebas\">Mohon Maaf...</h2>";
			printf("<div class=\"well\"><span class=\"err-icon\">Kami tidak dapat menemukan data untuk kueri pencarian '<u>%s</u>'!</span></div>", $query);
			
		}
	}
	
?>
</div>
