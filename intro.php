<!-- Halaman intro -->
<div class="container">
   <h1 class="text-center font-rabbid2">Dicariin Search</h1>
   <br/>
   <div class="col-sm-6 col-sm-offset-3">
      <div class="panel panel-default form-group">
         <div class="panel-heading"><strong>Apa yang ingin kamu cari?</strong></div>
         <div class="panel-body">
            <form name="formcari" class="form-inline" method="get" action="">
               <span style="width:65%;">
			     <input type="text" class="form-control" name="q" value="<?php echo $query; ?>" placeholder="Cari disini..." style="width:65%;">
               </span>
			   <span style="width:25%;">
			     <select class="form-control" name="max" style="width:25%;">
				   <option value="10">Top 10</option>
                   <option value="20" selected>Top 20</option>
                   <option value="25">Top 25</option>
                   <option value="50">Top 50</option>
                   <option value="75">Top 75</option>
                   <option value="100">Top 100</option>
                 </select>
			   </span>
			   <span style="width:10%;">
                 <button class="btn btn-default" type="submit">
                   <i class="glyphicon glyphicon-search"></i>
                 </button>
			   </span>
            </form>
         </div>
      </div>
   </div>
</div>
<br/>
