// Declare a unique namespace.
var vizObj = {};
var strArr = new Array(0);

// Class constructor. Parameter container is a DOM elementon the client that
// that will contain the visualization.
vizObj.ParallelC = function(container) {
  this.containerElement = container;
}

function drawAxes(ctx,data,canvasWidth,canvasHeight) {
	for (var col = 50; col < ((canvasWidth-50)); col = col + ((canvasWidth-50)/data.getNumberOfColumns())) 
	{
		ctx.strokeStyle = "#BDBDBD";
		ctx.beginPath();
		ctx.moveTo(col,30);
		ctx.lineTo(col,canvasHeight - 30);	
		ctx.closePath();
		ctx.stroke();
	}
}

function setMaxMin(ctx,data,canvasWidth,canvasHeight) {
	var x = 55;
	for (var col = 0; col < data.getNumberOfColumns(); col++) 
	{
		var colMax = 0;
		var colMin = 0;

		if (data.getColumnType(col) == "number")
		{
			colMax = data.getColumnRange(col).max;
			colMin = data.getColumnRange(col).min;
		}
		else
		{
			colMax = data.getDistinctValues(col).length-1;
			if (colMax == colMin)
			{
				colMin = -1;
			}
			else
			{
				colMin = 0;
			}
		}
			ctx.StrokeStyle = "#6E6E6E";

			ctx.beginPath();
			ctx.moveTo(x-10,50);
			ctx.lineTo(x,50);	
			ctx.closePath();
			ctx.stroke();
			
			try
			{
				ctx.font = "10px Verdana";
				if (data.getColumnType(col) == "number")
				{
					ctx.fillText(colMax,x,50);
				}
				else
				{
					//ctx.fillText(data.getDistinctValues(col)[data.getDistinctValues(col).length-1],x,50);
					ctx.fillText(data.getDistinctValues(col)[colMax],x,50);
				}
			}
			catch(e)
			{
				ctx.mozTextStyle = "10px Verdana";
				ctx.fillStyle = "black";
				ctx.translate(x,50);
				if (data.getColumnType(col) == "number")
				{
					ctx.mozDrawText(colMax);
				}
				else
				{
					//ctx.mozDrawText(data.getDistinctValues(col)[data.getDistinctValues(col).length-1]);
					ctx.mozDrawText(data.getDistinctValues(col)[colMax]);
				}
				ctx.translate(-x,-50);
			}
			
			if (colMin != -1 && (colMax != colMin))
			{
			//label for max
			ctx.beginPath();
			ctx.moveTo(x-10,canvasHeight-50);
			ctx.lineTo(x,canvasHeight-50);	
			ctx.closePath();
			ctx.stroke();
			
			try
			{
				ctx.font = "10px Verdana";
				if (data.getColumnType(col) == "number")
				{
					ctx.fillText(colMin,x,canvasHeight-45);
				}
				else
				{
					
					ctx.fillText(data.getDistinctValues(col)[0],x,canvasHeight-45);	
				}
			}
			catch(e)
			{
				ctx.mozTextStyle = "10px Verdana";
				ctx.fillStyle = "black";
				ctx.translate(x,canvasHeight-45);
				if (data.getColumnType(col) == "number")
				{
					ctx.mozDrawText(colMin);
				}
				else
				{
					//ctx.mozDrawText(data.getDistinctValues(col)[0]);
					ctx.mozDrawText(data.getDistinctValues(col)[colMin]);
				}
				ctx.translate(-x,-canvasHeight+45);
			}
			}
			if (data.getColumnType(col) != "number")
			{
				var y = 50;
				for (var i = 1; i < (data.getDistinctValues(col).length - 1) ; i++)
				{
					
					y = y + (((canvasHeight-100)/(data.getDistinctValues(col).length-1)));
					ctx.StrokeStyle = "#6E6E6E";
					ctx.beginPath();
					ctx.moveTo(x-10,y);
					ctx.lineTo(x,y);	
					ctx.closePath();
					ctx.stroke();
					
					try
					{
						ctx.font = "10px Verdana";
						if (data.getColumnType(col) == "number")
						{
							ctx.fillText(colMax,x,y);
						}
						else
						{
							ctx.fillText(data.getDistinctValues(col)[data.getDistinctValues(col).length-i-1],x,y);
						}
					}
					catch(e)
					{
						ctx.mozTextStyle = "10px Verdana";
						ctx.fillStyle = "black";
						ctx.translate(x,y);
						if (data.getColumnType(col) == "number")
						{
							ctx.mozDrawText(colMax);
						}
						else
						{
							ctx.mozDrawText(data.getDistinctValues(col)[data.getDistinctValues(col).length-i-1]);
						}
						ctx.translate(-x,-y);
					}

					//
				}
			}
			
		x= x + ((canvasWidth-50)/data.getNumberOfColumns());
	}

}

function normalizeTable(data) 
{
	for (var col = 0; col < data.getNumberOfColumns(); col++) 
	{
		if (data.getColumnType(col) == "number")
		{
			var step = data.getColumnRange(col);
			for (var row = 0; row < data.getNumberOfRows(); row++) 
			{
				data.setCell(row,col,data.getValue(row,col)-(step.min));		
			}
		}
	}
}
 
function plotGraph(ctx,data,options,canvasHeight,canvasWidth) 
{
	for (var row = 0; row < data.getNumberOfRows(); row++) 
	{
	if (options.lineColor == null)
	{
		ctx.strokeStyle = "#FE2E2E"; 
	}
	else
	{
		ctx.strokeStyle = options.lineColor; 
	}
	ctx.moveTo(50,50);
	var x = 50;
	
		for (var col = 1; col < data.getNumberOfColumns(); col++) 
		{
			
			var step = data.getColumnRange(col-1);			
			
			ctx.beginPath();		
			if (data.getColumnType(col-1) != "number")
			{
				var val = findDistinctColVal(data,row,col-1);
				if ((data.getDistinctValues(col-1).length-1) < 2)
				{
					ctx.moveTo(x,50);
				}
				else
				{
					ctx.moveTo(x,(canvasHeight)-(((val)/((data.getDistinctValues(col-1).length-1)/(canvasHeight-100)))+50));
				}
			}
			else
			{
				if (step.max == 0)
				{
					ctx.moveTo(x,50);
				}
				else
				{
					ctx.moveTo(x,(canvasHeight)-(((data.getValue(row, col-1))/((step.max)/(canvasHeight-100)))+50));	
				}
			}
			
			x= x + ((canvasWidth-50)/data.getNumberOfColumns());
			
			
			step = data.getColumnRange(col);
			if (data.getColumnType(col) != "number")
			{
				var val = findDistinctColVal(data,row,col);
				if ((data.getDistinctValues(col).length-1) < 2)
				{
					ctx.lineTo(x,50);				
				}
				else
				{
					ctx.lineTo(x,(canvasHeight)-(((val)/((data.getDistinctValues(col).length-1)/(canvasHeight-100)))+50));				
				}
			}
			else
			{
				if (step.max == 0)
				{
					ctx.lineTo(x,50);	
				}
				else
				{
					ctx.lineTo(x,(canvasHeight)-(((data.getValue(row, col))/((step.max)/(canvasHeight-100)))+50));	
				}
			}
			
			ctx.closePath();
			ctx.stroke();
		}

	}

}

function findDistinctColVal(data,row,col)
{
	for (var k = 0; k < data.getDistinctValues(col).length; k++)
	{
		if (data.getDistinctValues(col)[k] == data.getValue(row, col))
		{
			return k
		}
	}
}
function writeLabels(ctx,data,canvasHeight,canvasWidth) 
{
x = 30;
	for (var col = 0; col < data.getNumberOfColumns(); col++) 
	{
		
		ctx.fillStyle = "#6E6E6E";
		try
		{
			ctx.font = "10px Verdana";
			ctx.fillText(data.getColumnLabel(col),x,canvasHeight-5);
		}
		catch(e)
		{
			ctx.mozTextStyle = "10px Verdana";
			ctx.fillStyle = "#6E6E6E";
			ctx.translate(x,canvasHeight-5);
			ctx.mozDrawText(data.getColumnLabel(col));
			ctx.translate(-x,-canvasHeight+5);
		}
		x= x + ((canvasWidth-50)/data.getNumberOfColumns());
	}
}
 
function writeTitle(ctx,options,canvasHeight,canvasWidth)
{
		ctx.fillStyle = "black";
		try
		{
			ctx.font = "12px Verdana bold";
			ctx.fillText(options.title,(canvasWidth/2)-((options.title.length)/2)-100,10);
		}
		catch(e)
		{
			ctx.mozTextStyle = "12px Verdana";
			ctx.fillStyle = "black";
			ctx.translate((canvasWidth/2)-((options.title.length)/2)-100,20);
			ctx.mozDrawText(options.title);
			ctx.translate(-(canvasWidth/2)+((options.title.length)/2)+100,-20);
			
		}
}
// Main drawing logic.
vizObj.ParallelC.prototype.draw = function(data, options) {
	
	this.containerElement.innerHTML = '<canvas id="graph_parallelC"></canvas>';
	//canvas properties
	
	if (options.width == undefined)
	{
		canvasWidth = 500;
	}
	else
	{
		canvasWidth = options.width;
	}
	if (options.height == undefined)
	{
		canvasHeight = 500;
	}
	else
	{
		canvasHeight = options.height;
	}
	
	document.getElementById("graph_parallelC").setAttribute("width",canvasWidth);
	document.getElementById("graph_parallelC").setAttribute("height",canvasHeight);
	
	var canvas = document.getElementById("graph_parallelC");
	
  if (canvas.getContext)
  {
	//var strArr = new Array(1);
	var ctx = canvas.getContext('2d');
	ctx.lineWidth=1;
	
	//drawing axes
	drawAxes(ctx,data,canvasWidth,canvasHeight);
	
	
	//set max & min points on each axes
	setMaxMin(ctx,data,canvasWidth,canvasHeight);
	
	//normalizing data
	normalizeTable(data);
	
	//plotting data
	plotGraph(ctx,data,options,canvasHeight,canvasWidth);
	
	//adding labels
	writeLabels(ctx,data,canvasHeight,canvasWidth);

	//adding chart title
	writeTitle(ctx,options,canvasHeight,canvasWidth);
	
	
  }
  else
  {
	this.containerElement.innerHTML = '<span>This feature is not compatible with the brower you are currently using.</span>';
  }
  
  
}

// Utility function to escape HTML special characters
vizObj.ParallelC.prototype.escapeHtml = function(text) {
  if (text == null)
    return '';
  return text.replace(/&/g, '&').replace(/</g, '<')
      .replace(/>/g, '>').replace(/"/g, '"');
} 
